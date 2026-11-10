/*
 * File: DCPS_model.h
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

#ifndef DCPS_model_h_
#define DCPS_model_h_
#ifndef DCPS_model_COMMON_INCLUDES_
#define DCPS_model_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                                 /* DCPS_model_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_DCPS_model_T RT_MODEL_DCPS_model_T;

/* Block signals (default storage) */
typedef struct {
  real32_T Saturation;                 /* '<S2>/Saturation' */
  real32_T Saturation_f;               /* '<S1>/Saturation' */
} B_DCPS_model_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real32_T Filter_DSTATE;              /* '<S86>/Filter' */
  real32_T Integrator_DSTATE;          /* '<S91>/Integrator' */
  real32_T Filter_DSTATE_d;            /* '<S33>/Filter' */
  real32_T Integrator_DSTATE_p;        /* '<S38>/Integrator' */
} DW_DCPS_model_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real32_T state;                      /* '<Root>/state' */
  real32_T V_target;                   /* '<Root>/V_target' */
  real32_T V_value;                    /* '<Root>/V_value' */
  real32_T I_target;                   /* '<Root>/I_target' */
  real32_T I_value;                    /* '<Root>/I_value' */
} ExtU_DCPS_model_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real32_T duty[2];                    /* '<Root>/duty' */
} ExtY_DCPS_model_T;

/* Real-time Model Data Structure */
struct tag_RTM_DCPS_model_T {
  const char_T * volatile errorStatus;
};

/* Block signals (default storage) */
extern B_DCPS_model_T DCPS_model_B;

/* Block states (default storage) */
extern DW_DCPS_model_T DCPS_model_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_DCPS_model_T DCPS_model_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_DCPS_model_T DCPS_model_Y;

/* Model entry point functions */
extern void DCPS_model_initialize(void);
extern void DCPS_model_step(void);
extern void DCPS_model_terminate(void);

/* Real-time Model object */
extern RT_MODEL_DCPS_model_T *const DCPS_model_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S1>/Scope' : Unused code path elimination
 * Block '<S2>/Scope' : Unused code path elimination
 * Block '<S35>/Integral Gain' : Eliminated nontunable gain of 1
 * Block '<S88>/Integral Gain' : Eliminated nontunable gain of 1
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'DCPS_model'
 * '<S1>'   : 'DCPS_model/CV'
 * '<S2>'   : 'DCPS_model/CV1'
 * '<S3>'   : 'DCPS_model/CV/PID Controller'
 * '<S4>'   : 'DCPS_model/CV/PID Controller/Anti-windup'
 * '<S5>'   : 'DCPS_model/CV/PID Controller/D Gain'
 * '<S6>'   : 'DCPS_model/CV/PID Controller/External Derivative'
 * '<S7>'   : 'DCPS_model/CV/PID Controller/Filter'
 * '<S8>'   : 'DCPS_model/CV/PID Controller/Filter ICs'
 * '<S9>'   : 'DCPS_model/CV/PID Controller/I Gain'
 * '<S10>'  : 'DCPS_model/CV/PID Controller/Ideal P Gain'
 * '<S11>'  : 'DCPS_model/CV/PID Controller/Ideal P Gain Fdbk'
 * '<S12>'  : 'DCPS_model/CV/PID Controller/Integrator'
 * '<S13>'  : 'DCPS_model/CV/PID Controller/Integrator ICs'
 * '<S14>'  : 'DCPS_model/CV/PID Controller/N Copy'
 * '<S15>'  : 'DCPS_model/CV/PID Controller/N Gain'
 * '<S16>'  : 'DCPS_model/CV/PID Controller/P Copy'
 * '<S17>'  : 'DCPS_model/CV/PID Controller/Parallel P Gain'
 * '<S18>'  : 'DCPS_model/CV/PID Controller/Reset Signal'
 * '<S19>'  : 'DCPS_model/CV/PID Controller/Saturation'
 * '<S20>'  : 'DCPS_model/CV/PID Controller/Saturation Fdbk'
 * '<S21>'  : 'DCPS_model/CV/PID Controller/Sum'
 * '<S22>'  : 'DCPS_model/CV/PID Controller/Sum Fdbk'
 * '<S23>'  : 'DCPS_model/CV/PID Controller/Tracking Mode'
 * '<S24>'  : 'DCPS_model/CV/PID Controller/Tracking Mode Sum'
 * '<S25>'  : 'DCPS_model/CV/PID Controller/Tsamp - Integral'
 * '<S26>'  : 'DCPS_model/CV/PID Controller/Tsamp - Ngain'
 * '<S27>'  : 'DCPS_model/CV/PID Controller/postSat Signal'
 * '<S28>'  : 'DCPS_model/CV/PID Controller/preInt Signal'
 * '<S29>'  : 'DCPS_model/CV/PID Controller/preSat Signal'
 * '<S30>'  : 'DCPS_model/CV/PID Controller/Anti-windup/Passthrough'
 * '<S31>'  : 'DCPS_model/CV/PID Controller/D Gain/Internal Parameters'
 * '<S32>'  : 'DCPS_model/CV/PID Controller/External Derivative/Error'
 * '<S33>'  : 'DCPS_model/CV/PID Controller/Filter/Disc. Forward Euler Filter'
 * '<S34>'  : 'DCPS_model/CV/PID Controller/Filter ICs/Internal IC - Filter'
 * '<S35>'  : 'DCPS_model/CV/PID Controller/I Gain/Internal Parameters'
 * '<S36>'  : 'DCPS_model/CV/PID Controller/Ideal P Gain/Passthrough'
 * '<S37>'  : 'DCPS_model/CV/PID Controller/Ideal P Gain Fdbk/Disabled'
 * '<S38>'  : 'DCPS_model/CV/PID Controller/Integrator/Discrete'
 * '<S39>'  : 'DCPS_model/CV/PID Controller/Integrator ICs/Internal IC'
 * '<S40>'  : 'DCPS_model/CV/PID Controller/N Copy/Disabled'
 * '<S41>'  : 'DCPS_model/CV/PID Controller/N Gain/Internal Parameters'
 * '<S42>'  : 'DCPS_model/CV/PID Controller/P Copy/Disabled'
 * '<S43>'  : 'DCPS_model/CV/PID Controller/Parallel P Gain/Internal Parameters'
 * '<S44>'  : 'DCPS_model/CV/PID Controller/Reset Signal/Disabled'
 * '<S45>'  : 'DCPS_model/CV/PID Controller/Saturation/Passthrough'
 * '<S46>'  : 'DCPS_model/CV/PID Controller/Saturation Fdbk/Disabled'
 * '<S47>'  : 'DCPS_model/CV/PID Controller/Sum/Sum_PID'
 * '<S48>'  : 'DCPS_model/CV/PID Controller/Sum Fdbk/Disabled'
 * '<S49>'  : 'DCPS_model/CV/PID Controller/Tracking Mode/Disabled'
 * '<S50>'  : 'DCPS_model/CV/PID Controller/Tracking Mode Sum/Passthrough'
 * '<S51>'  : 'DCPS_model/CV/PID Controller/Tsamp - Integral/TsSignalSpecification'
 * '<S52>'  : 'DCPS_model/CV/PID Controller/Tsamp - Ngain/Passthrough'
 * '<S53>'  : 'DCPS_model/CV/PID Controller/postSat Signal/Forward_Path'
 * '<S54>'  : 'DCPS_model/CV/PID Controller/preInt Signal/Internal PreInt'
 * '<S55>'  : 'DCPS_model/CV/PID Controller/preSat Signal/Forward_Path'
 * '<S56>'  : 'DCPS_model/CV1/PID Controller'
 * '<S57>'  : 'DCPS_model/CV1/PID Controller/Anti-windup'
 * '<S58>'  : 'DCPS_model/CV1/PID Controller/D Gain'
 * '<S59>'  : 'DCPS_model/CV1/PID Controller/External Derivative'
 * '<S60>'  : 'DCPS_model/CV1/PID Controller/Filter'
 * '<S61>'  : 'DCPS_model/CV1/PID Controller/Filter ICs'
 * '<S62>'  : 'DCPS_model/CV1/PID Controller/I Gain'
 * '<S63>'  : 'DCPS_model/CV1/PID Controller/Ideal P Gain'
 * '<S64>'  : 'DCPS_model/CV1/PID Controller/Ideal P Gain Fdbk'
 * '<S65>'  : 'DCPS_model/CV1/PID Controller/Integrator'
 * '<S66>'  : 'DCPS_model/CV1/PID Controller/Integrator ICs'
 * '<S67>'  : 'DCPS_model/CV1/PID Controller/N Copy'
 * '<S68>'  : 'DCPS_model/CV1/PID Controller/N Gain'
 * '<S69>'  : 'DCPS_model/CV1/PID Controller/P Copy'
 * '<S70>'  : 'DCPS_model/CV1/PID Controller/Parallel P Gain'
 * '<S71>'  : 'DCPS_model/CV1/PID Controller/Reset Signal'
 * '<S72>'  : 'DCPS_model/CV1/PID Controller/Saturation'
 * '<S73>'  : 'DCPS_model/CV1/PID Controller/Saturation Fdbk'
 * '<S74>'  : 'DCPS_model/CV1/PID Controller/Sum'
 * '<S75>'  : 'DCPS_model/CV1/PID Controller/Sum Fdbk'
 * '<S76>'  : 'DCPS_model/CV1/PID Controller/Tracking Mode'
 * '<S77>'  : 'DCPS_model/CV1/PID Controller/Tracking Mode Sum'
 * '<S78>'  : 'DCPS_model/CV1/PID Controller/Tsamp - Integral'
 * '<S79>'  : 'DCPS_model/CV1/PID Controller/Tsamp - Ngain'
 * '<S80>'  : 'DCPS_model/CV1/PID Controller/postSat Signal'
 * '<S81>'  : 'DCPS_model/CV1/PID Controller/preInt Signal'
 * '<S82>'  : 'DCPS_model/CV1/PID Controller/preSat Signal'
 * '<S83>'  : 'DCPS_model/CV1/PID Controller/Anti-windup/Passthrough'
 * '<S84>'  : 'DCPS_model/CV1/PID Controller/D Gain/Internal Parameters'
 * '<S85>'  : 'DCPS_model/CV1/PID Controller/External Derivative/Error'
 * '<S86>'  : 'DCPS_model/CV1/PID Controller/Filter/Disc. Forward Euler Filter'
 * '<S87>'  : 'DCPS_model/CV1/PID Controller/Filter ICs/Internal IC - Filter'
 * '<S88>'  : 'DCPS_model/CV1/PID Controller/I Gain/Internal Parameters'
 * '<S89>'  : 'DCPS_model/CV1/PID Controller/Ideal P Gain/Passthrough'
 * '<S90>'  : 'DCPS_model/CV1/PID Controller/Ideal P Gain Fdbk/Disabled'
 * '<S91>'  : 'DCPS_model/CV1/PID Controller/Integrator/Discrete'
 * '<S92>'  : 'DCPS_model/CV1/PID Controller/Integrator ICs/Internal IC'
 * '<S93>'  : 'DCPS_model/CV1/PID Controller/N Copy/Disabled'
 * '<S94>'  : 'DCPS_model/CV1/PID Controller/N Gain/Internal Parameters'
 * '<S95>'  : 'DCPS_model/CV1/PID Controller/P Copy/Disabled'
 * '<S96>'  : 'DCPS_model/CV1/PID Controller/Parallel P Gain/Internal Parameters'
 * '<S97>'  : 'DCPS_model/CV1/PID Controller/Reset Signal/Disabled'
 * '<S98>'  : 'DCPS_model/CV1/PID Controller/Saturation/Passthrough'
 * '<S99>'  : 'DCPS_model/CV1/PID Controller/Saturation Fdbk/Disabled'
 * '<S100>' : 'DCPS_model/CV1/PID Controller/Sum/Sum_PID'
 * '<S101>' : 'DCPS_model/CV1/PID Controller/Sum Fdbk/Disabled'
 * '<S102>' : 'DCPS_model/CV1/PID Controller/Tracking Mode/Disabled'
 * '<S103>' : 'DCPS_model/CV1/PID Controller/Tracking Mode Sum/Passthrough'
 * '<S104>' : 'DCPS_model/CV1/PID Controller/Tsamp - Integral/TsSignalSpecification'
 * '<S105>' : 'DCPS_model/CV1/PID Controller/Tsamp - Ngain/Passthrough'
 * '<S106>' : 'DCPS_model/CV1/PID Controller/postSat Signal/Forward_Path'
 * '<S107>' : 'DCPS_model/CV1/PID Controller/preInt Signal/Internal PreInt'
 * '<S108>' : 'DCPS_model/CV1/PID Controller/preSat Signal/Forward_Path'
 */
#endif                                 /* DCPS_model_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
