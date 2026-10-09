#ifndef INA226_H
#define INA226_H

#include "stm32g4xx_hal.h"

uint8_t ina226_init(I2C_HandleTypeDef *hi2c);
float ina226_getbusvoltage_V(I2C_HandleTypeDef *hi2c);
float ina226_getshuntvoltage_V(I2C_HandleTypeDef *hi2c);
float ina226_getcurrent_A(I2C_HandleTypeDef *hi2c);
float ina226_getpower_W(I2C_HandleTypeDef *hi2c);
float ina226_calpower_W(I2C_HandleTypeDef *hi2c);

uint8_t adc_init(void);
float adc_getvoltageoutput_v(void);
float adc_getvoltageinput_v(void);

#endif
