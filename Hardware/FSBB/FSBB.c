#include "FSBB.h"
#include "main.h"
#include "stm32g4xx_hal_gpio.h"
#include "tim.h"
#include "gpio.h"
#include "DCPS_model.h"
#include <stdint.h>

#define BUCK_TIM_HANDLER htim8
#define BUCK_TIM_CHANNEL TIM_CHANNEL_1
#define BOOST_TIM_HANDLER htim4
#define BOOST_TIM_CHANNEL TIM_CHANNEL_2

#define ALWAYS_ON_DUTY 0.99f

extern volatile float input_voltage;
extern volatile float output_voltage;
extern volatile float current;
fsbb_mode_t fsbb_mode = MODE_CV;

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

void fsbb_start(float buck_duty, float boost_duty) {
  uint32_t arr1 = TIM8->ARR;
  uint32_t arr2 = TIM4->ARR;

  buck_duty = _constrainduty(buck_duty);
  boost_duty = _constrainduty(boost_duty);

  uint32_t ccr1 = (uint32_t)((float)arr1 * buck_duty);
  uint32_t ccr2 = (uint32_t)((float)arr2 * boost_duty);

  __HAL_TIM_SET_COMPARE(&BUCK_TIM_HANDLER, BUCK_TIM_CHANNEL, ccr1);
  __HAL_TIM_SET_COMPARE(&BOOST_TIM_HANDLER, BOOST_TIM_CHANNEL, ccr2);

  HAL_GPIO_WritePin(SD1_GPIO_Port, SD1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(SD2_GPIO_Port, SD2_Pin, GPIO_PIN_SET);
  HAL_TIM_PWM_Start(&BUCK_TIM_HANDLER, BUCK_TIM_CHANNEL);
  HAL_TIM_PWM_Start(&BOOST_TIM_HANDLER, BOOST_TIM_CHANNEL);
}

void fsbb_init(void) {
  fsbb_stop();
  DCPS_model_initialize();
  fsbb_mode = MODE_CV;
  DCPS_model_U.state = fsbb_mode;
  DCPS_model_U.V_target = 12.0f;
  DCPS_model_U.I_target = 1.0f;
}

void fsbb_run(void) {
  float ctrl_duty;
  float buck_duty;
  float boost_duty;

  DCPS_model_U.V_value = output_voltage;
  DCPS_model_U.I_value = current;

  DCPS_model_step();

  ctrl_duty = ((int32_t)DCPS_model_U.state == 1)? DCPS_model_Y.duty[0]: DCPS_model_Y.duty[1];

  if (DCPS_model_U.V_target <= input_voltage) {
    buck_duty = ctrl_duty;
    boost_duty = ALWAYS_ON_DUTY;
  } else {
    buck_duty = ALWAYS_ON_DUTY;
    boost_duty = ctrl_duty;
  }

  fsbb_start(buck_duty, boost_duty);
}

