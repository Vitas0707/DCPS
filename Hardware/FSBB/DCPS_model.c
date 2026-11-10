/*
 * File: DCPS_model.c
 *
 * Code generated for Simulink model 'DCPS_model'.
 *
 * Model version                  : 1.3
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Sun Nov  8 11:06:40 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom Processor
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "DCPS_model.h"
#include <math.h>
#include "rtwtypes.h"
#include "math.h"

/* Block signals (default storage) */
B_DCPS_model_T DCPS_model_B;

/* Block states (default storage) */
DW_DCPS_model_T DCPS_model_DW;

/* External inputs (root inport signals with default storage) */
ExtU_DCPS_model_T DCPS_model_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_DCPS_model_T DCPS_model_Y;

/* Real-time model */
static RT_MODEL_DCPS_model_T DCPS_model_M_;
RT_MODEL_DCPS_model_T *const DCPS_model_M = &DCPS_model_M_;
extern real_T rtInf;
extern real_T rtMinusInf;
extern real_T rtNaN;
extern real32_T rtInfF;
extern real32_T rtMinusInfF;
extern real32_T rtNaNF;
static boolean_T rtIsInf(real_T value);
static boolean_T rtIsInfF(real32_T value);
static boolean_T rtIsNaN(real_T value);
static boolean_T rtIsNaNF(real32_T value);
real_T rtNaN = -(real_T)NAN;
real_T rtInf = (real_T)INFINITY;
real_T rtMinusInf = -(real_T)INFINITY;
real32_T rtNaNF = -(real32_T)NAN;
real32_T rtInfF = (real32_T)INFINITY;
real32_T rtMinusInfF = -(real32_T)INFINITY;

/* Test if value is infinite */
static boolean_T rtIsInf(real_T value)
{
  return (boolean_T)isinf(value);
}

/* Test if single-precision value is infinite */
static boolean_T rtIsInfF(real32_T value)
{
  return (boolean_T)isinf(value);
}

/* Test if value is not a number */
static boolean_T rtIsNaN(real_T value)
{
  return (boolean_T)(isnan(value) != 0);
}

/* Test if single-precision value is not a number */
static boolean_T rtIsNaNF(real32_T value)
{
  return (boolean_T)(isnan(value) != 0);
}

/* Model step function */
void DCPS_model_step(void)
{
  real32_T rtb_FilterCoefficient;
  real32_T rtb_Sum;

  /* SwitchCase: '<Root>/Switch Case' incorporates:
   *  Inport: '<Root>/state'
   */
  rtb_Sum = truncf(DCPS_model_U.state);
  if (rtIsNaNF(rtb_Sum) || rtIsInfF(rtb_Sum)) {
    rtb_Sum = 0.0F;
  } else {
    rtb_Sum = fmodf(rtb_Sum, 4.2949673E+9F);
  }
  //state = 1进入恒压模式，其余默认进恒流模式
  if ((rtb_Sum < 0.0F ? -(int32_T)(uint32_T)-rtb_Sum : (int32_T)(uint32_T)
       rtb_Sum) == 1) {
    /* Outputs for IfAction SubSystem: '<Root>/CV' incorporates:
     *  ActionPort: '<S1>/Action Port'
     */
    /* Sum: '<S1>/Sum' incorporates:
     *  Inport: '<Root>/V_target'
     *  Inport: '<Root>/V_value'
     */
    rtb_Sum = DCPS_model_U.V_target - DCPS_model_U.V_value;

    /* Gain: '<S41>/Filter Coefficient' incorporates:
     *  DiscreteIntegrator: '<S33>/Filter'
     *  Gain: '<S31>/Derivative Gain'
     *  Sum: '<S33>/SumD'
     */
    rtb_FilterCoefficient = (DCPS_model_DW.V_Kd * rtb_Sum - DCPS_model_DW.Filter_DSTATE_d) *
      100.0F;

    /* Sum: '<S47>/Sum' incorporates:
     *  DiscreteIntegrator: '<S38>/Integrator'
     *  Gain: '<S43>/Proportional Gain'
     */
    DCPS_model_B.Saturation_f = (DCPS_model_DW.V_Kp * rtb_Sum +
      DCPS_model_DW.Integrator_DSTATE_p) + rtb_FilterCoefficient;

    /* Saturate: '<S1>/Saturation' */
    if (DCPS_model_B.Saturation_f > 0.99F) {
      /* Sum: '<S47>/Sum' incorporates:
       *  Saturate: '<S1>/Saturation'
       */
      DCPS_model_B.Saturation_f = 0.99F;
    } else if (DCPS_model_B.Saturation_f < 0.01F) {
      /* Sum: '<S47>/Sum' incorporates:
       *  Saturate: '<S1>/Saturation'
       */
      DCPS_model_B.Saturation_f = 0.01F;
    }

    /* End of Saturate: '<S1>/Saturation' */

    /* Update for DiscreteIntegrator: '<S33>/Filter' */
    DCPS_model_DW.Filter_DSTATE_d += 0.002F * rtb_FilterCoefficient;

    /* Update for DiscreteIntegrator: '<S38>/Integrator' */
    DCPS_model_DW.Integrator_DSTATE_p += DCPS_model_DW.V_Ki*0.002F * rtb_Sum;

    /* End of Outputs for SubSystem: '<Root>/CV' */
  } else {
    /* Outputs for IfAction SubSystem: '<Root>/CC' incorporates:
     *  ActionPort: '<S2>/Action Port'
     */
    /* Sum: '<S2>/Sum' incorporates:
     *  Inport: '<Root>/I_target'
     *  Inport: '<Root>/I_value'
     */
    rtb_Sum = DCPS_model_U.I_target - DCPS_model_U.I_value;

    /* Gain: '<S94>/Filter Coefficient' incorporates:
     *  DiscreteIntegrator: '<S86>/Filter'
     *  Gain: '<S84>/Derivative Gain'
     *  Sum: '<S86>/SumD'
     */
    rtb_FilterCoefficient = (DCPS_model_DW.I_Kd * rtb_Sum - DCPS_model_DW.Filter_DSTATE) *
      100.0F;

    /* Sum: '<S100>/Sum' incorporates:
     *  DiscreteIntegrator: '<S91>/Integrator'
     *  Gain: '<S96>/Proportional Gain'
     */
    DCPS_model_B.Saturation = (DCPS_model_DW.I_Kp * rtb_Sum + DCPS_model_DW.Integrator_DSTATE)
      + rtb_FilterCoefficient;

    /* Saturate: '<S2>/Saturation' */
    if (DCPS_model_B.Saturation > 0.99F) {
      /* Sum: '<S100>/Sum' incorporates:
       *  Saturate: '<S2>/Saturation'
       */
      DCPS_model_B.Saturation = 0.99F;
    } else if (DCPS_model_B.Saturation < 0.01F) {
      /* Sum: '<S100>/Sum' incorporates:
       *  Saturate: '<S2>/Saturation'
       */
      DCPS_model_B.Saturation = 0.01F;
    }

    /* End of Saturate: '<S2>/Saturation' */

    /* Update for DiscreteIntegrator: '<S86>/Filter' */
    DCPS_model_DW.Filter_DSTATE += 0.002F * rtb_FilterCoefficient;

    /* Update for DiscreteIntegrator: '<S91>/Integrator' */
    DCPS_model_DW.Integrator_DSTATE += DCPS_model_DW.I_Ki*0.002F * rtb_Sum;

    /* End of Outputs for SubSystem: '<Root>/CV1' */
  }

  /* End of SwitchCase: '<Root>/Switch Case' */

  /* Outport: '<Root>/duty' */
  DCPS_model_Y.duty[0] = DCPS_model_B.Saturation_f;
  DCPS_model_Y.duty[1] = DCPS_model_B.Saturation;
}

/* Model initialize function */
void DCPS_model_initialize(void)
{
  DCPS_model_DW.V_Kd = 0.1f;
  DCPS_model_DW.V_Ki = 1.0f;
  DCPS_model_DW.V_Kp = 5.0f;
  DCPS_model_DW.I_Kp = 5.0f;
  DCPS_model_DW.I_Ki = 1.0f;
  DCPS_model_DW.I_Kd = 0.1f;
  DCPS_model_DW.Filter_DSTATE_d = 0.0f;
  DCPS_model_DW.Integrator_DSTATE_p = 0.0f;
  DCPS_model_DW.Filter_DSTATE = 0.0f;
  DCPS_model_DW.Integrator_DSTATE = 0.0f;
}

/* Model terminate function */
void DCPS_model_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
