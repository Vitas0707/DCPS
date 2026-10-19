#include "FSBB.h"
#include "main.h"
#include "stm32g4xx_hal_gpio.h"
#include "tim.h"
#include "gpio.h"
#include <stdint.h>

#define BUCK_TIM_HANDLER htim8
#define BUCK_TIM_CHANNEL TIM_CHANNEL_1
#define BOOST_TIM_HANDLER htim4
#define BOOST_TIM_CHANNEL TIM_CHANNEL_2

#define ALWAYS_ON_DUTY 0.99f

static float _constrainduty(float duty) {
  if (duty < 0.01f) {
    return 0.01f;
  }
  else if (duty > 0.99f) {
    return 0.99f;
  }
  else {
    return duty;
  }
}

void fsbb_debugon(void) {
  uint32_t arr1 = TIM8->ARR;
  uint32_t arr2 = TIM4->ARR;
  float debug_duty = 0.50f;

  uint32_t ccr1 = (uint32_t)((float)arr1 * debug_duty);
  uint32_t ccr2 = (uint32_t)((float)arr2 * ALWAYS_ON_DUTY);

  __HAL_TIM_SET_COMPARE(&BUCK_TIM_HANDLER, BUCK_TIM_CHANNEL, ccr1);
  __HAL_TIM_SET_COMPARE(&BOOST_TIM_HANDLER, BOOST_TIM_CHANNEL, ccr2);

  HAL_GPIO_WritePin(SD1_GPIO_Port, SD1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(SD2_GPIO_Port, SD2_Pin, GPIO_PIN_SET);

  HAL_TIM_PWM_Start(&BUCK_TIM_HANDLER, BUCK_TIM_CHANNEL);
  HAL_TIM_PWM_Start(&BOOST_TIM_HANDLER, BOOST_TIM_CHANNEL);
}

void fsbb_stop(void) {
  HAL_GPIO_WritePin(SD1_GPIO_Port, SD1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SD2_GPIO_Port, SD2_Pin, GPIO_PIN_RESET);

  __HAL_TIM_SET_COMPARE(&BUCK_TIM_HANDLER, BUCK_TIM_CHANNEL, 0);
  __HAL_TIM_SET_COMPARE(&BOOST_TIM_HANDLER, BOOST_TIM_CHANNEL, 0);

  HAL_TIM_PWM_Stop(&BUCK_TIM_HANDLER, BUCK_TIM_CHANNEL);
  HAL_TIM_PWM_Stop(&BOOST_TIM_HANDLER, BOOST_TIM_CHANNEL);
}