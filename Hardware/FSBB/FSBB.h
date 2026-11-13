#ifndef FSBB_H
#define FSBB_H

typedef enum {
  MODE_CC = 0,
  MODE_CV = 1,
  MODE_STOP
} fsbb_mode_t;

void fsbb_debugon(void);
void fsbb_stop(void);
void fsbb_start(float buck_duty, float boost_duty);

void fsbb_init(void);
void fsbb_run(void);

#endif
