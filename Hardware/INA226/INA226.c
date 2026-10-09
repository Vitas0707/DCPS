#include "INA226.h"
#include "main.h"
#include "stm32g431xx.h"
#include "stm32g4xx_hal_adc.h"
#include "stm32g4xx_hal_adc_ex.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_i2c.h"
#include <stdint.h>

//INA226驱动部分
#define INA226_ADDRESS 0x40 // 7-bit address
#define INA226_WRITE_ADDRESS (INA226_ADDRESS << 1)
#define INA226_READ_ADDRESS (INA226_WRITE_ADDRESS | 0x01)
#define INA226_I2C_TIMEOUT 100

#define REG_CONFIG 0x00
#define REG_SHUNT_VOLT 0x01
#define REG_BUS_VOLT 0x02
#define REG_POWER 0x03
#define REG_CURRENT 0x04
#define REG_CALIBRATION 0x05

#define RSENSE (0.001f)
#define MAX_EXPECTED_CURRENT (3.0f)
#define CURRENT_LSB (MAX_EXPECTED_CURRENT / 32768.0f)
#define CAL ((uint16_t)(0.00512f / (CURRENT_LSB * RSENSE)))

// ADC测量电压部分
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern OPAMP_HandleTypeDef hopamp1;
extern OPAMP_HandleTypeDef hopamp2;

#define RATIO ((10.0f) / (10.0f + 100.0f)) // 分压比
#define ADC_VREF 2.5f                      // ADC参考电压
#define ADC_RESOLUTION 4096.0f             // 12位ADC分辨率
#define ADC_SAMPLE_TIMES 10.0f             // ADC采样次数--->均值滤波

/*INA226驱动代码*/
static HAL_StatusTypeDef INA226_wirtereg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t value) {
    uint8_t data[2];
    data[1] = (value >> 8) & 0xFF; // MSB
    data[2] = value & 0xFF;        // LSB

    return HAL_I2C_Mem_Write(hi2c, INA226_WRITE_ADDRESS, reg, 1, data, 2, INA226_I2C_TIMEOUT);
}

static HAL_StatusTypeDef INA226_readreg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t *value) {
    uint8_t data[2];
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, INA226_READ_ADDRESS, reg, 1, data, 2, INA226_I2C_TIMEOUT);
    if (status == HAL_OK) {
        *value = (data[0] << 8) | data[1]; // Combine MSB and LSB
    }
    return status;
}


uint8_t ina226_init(I2C_HandleTypeDef *hi2c) {
    uint16_t config = 0x4127; // default configuration
    if (INA226_wirtereg(hi2c, REG_CONFIG, config) != HAL_OK) {
        return 1;
    }
    if (INA226_wirtereg(hi2c, REG_CALIBRATION, CAL) != HAL_OK) {
        return 2;
    }
    return 0;
}

static uint16_t ina226_ReadShuntVoltage(I2C_HandleTypeDef *hi2c) {
    uint16_t value;
    INA226_readreg(hi2c, REG_SHUNT_VOLT, &value);
    return value;
}

static uint16_t ina226_ReadBusVoltage(I2C_HandleTypeDef *hi2c) {
    uint16_t value;
    INA226_readreg(hi2c, REG_BUS_VOLT, &value);
    return value;
}

static uint16_t ina226_ReadCurrent(I2C_HandleTypeDef *hi2c) {
    uint16_t value;
    INA226_readreg(hi2c, REG_CURRENT, &value);
    return value;
}

static uint16_t ina226_ReadPower(I2C_HandleTypeDef *hi2c) {
    uint16_t value;
    INA226_readreg(hi2c, REG_POWER, &value);
    return value;
}

float ina226_getcurrent_A(I2C_HandleTypeDef *hi2c) {
    uint16_t raw_current = ina226_ReadCurrent(hi2c);
    return (float)raw_current * CURRENT_LSB; // Convert to Amperes
}

float ina226_getshuntvoltage_V(I2C_HandleTypeDef *hi2c) {
    uint16_t raw_shunt_voltage = ina226_ReadShuntVoltage(hi2c);
    return (float)raw_shunt_voltage * 2.5e-6f; // Convert to Volts fixed LSB value 2.5uV
}

float ina226_getpower_W(I2C_HandleTypeDef *hi2c) {
    uint16_t raw_power = ina226_ReadPower(hi2c);
    return (float)raw_power * CURRENT_LSB * 25.0f; // Convert to Watts
}

float ina226_getbusvoltage_V(I2C_HandleTypeDef *hi2c) {
    uint16_t raw_bus_voltage = ina226_ReadBusVoltage(hi2c);
    return (float)raw_bus_voltage * 0.00125f; // Convert to Volts fixed LSB value 1.25mV
}

float ina226_calpower_W(I2C_HandleTypeDef *hi2c) {
  //手动计算，防止溢出
    float current = ina226_getcurrent_A(hi2c);
    float bus_voltage = ina226_getbusvoltage_V(hi2c);
    return current * bus_voltage; // Power in Watts
}

/*ADC测量输入输出电压代码*/
uint8_t adc_init(void) {
    HAL_ADCEx_Calibration_GetValue(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_GetValue(&hadc2, ADC_SINGLE_ENDED);
    if(HAL_OPAMP_Start(&hopamp1) != HAL_OK || HAL_OPAMP_Start(&hopamp2) != HAL_OK) {
        return 1; // OPAMP开启失败
    }
    if ((HAL_ADC_Start(&hadc1) != HAL_OK)||(HAL_ADC_Start(&hadc2) != HAL_OK)) {
        return 2; // ADC开启失败
    }
    return 0; // 初始化成功
}

float adc_getvoltageoutput_v(void) {
    uint32_t sum = 0;
    for (int i = 0; i < ADC_SAMPLE_TIMES; i++) {
        if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
            sum += HAL_ADC_GetValue(&hadc1);
        }
    }
    return (float)sum / ADC_SAMPLE_TIMES *(ADC_VREF / ADC_RESOLUTION); // Convert to Volts
}
float adc_getvoltageinput_v(void) {
    uint32_t sum = 0;
    for (int i = 0; i < ADC_SAMPLE_TIMES; i++) {
        if (HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY) == HAL_OK) {
            sum += HAL_ADC_GetValue(&hadc2);
        }
    }
    return ((float)sum / ADC_SAMPLE_TIMES *(ADC_VREF / ADC_RESOLUTION)) / RATIO; // Convert to Volts
}