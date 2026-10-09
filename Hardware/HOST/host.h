#ifndef HOST_H
#define HOST_H

#include "main.h"

uint8_t vofa_sendjustfloat(UART_HandleTypeDef *huart, const float *data,uint8_t length) ;

#endif // HOST_H
