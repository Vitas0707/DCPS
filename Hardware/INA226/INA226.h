#ifndef INA226_H
#define INA226_H

#include "stm32g4xx_hal.h"

uint8_t ina226_init(void);
float ina226_getbusvoltage_V(void);
float ina226_getshuntvoltage_V(void);
float ina226_getcurrent_A(void);
float ina226_getpower_W(void);
float ina226_calpower_W(void);

uint8_t adc_start(void);
float adc_getvoltageoutput_v(void);
float adc_getvoltageinput_v(void);

#endif
