#include "main.h"
#include "stm32g4xx_hal_uart.h"
#include <stdint.h>

#define UART_TIMEOUT 100

uint8_t vofa_sendjustfloat(UART_HandleTypeDef *huart, const float *data,uint8_t length) {
    uint8_t tail[4] = {0x00,0x00,0x80,0x7f};
    if(data == NULL || length == 0) {
        return 1; // Invalid input
    }
    uint16_t data_bytes = length * sizeof(float);
    if (HAL_UART_Transmit(huart, (const uint8_t*)data, data_bytes, UART_TIMEOUT) != HAL_OK) {
        return 2; // Error in transmission
    }
    if (HAL_UART_Transmit(huart, tail, sizeof(tail), UART_TIMEOUT) != HAL_OK) {
        return 3; // Error in transmission
    }
    return 0; // Success
}