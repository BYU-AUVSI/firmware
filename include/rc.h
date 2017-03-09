#ifndef RC_H_
#define RC_H_

#include <stdint.h>
#include <stdbool.h>

#include "mux.h"
#include "param.h"

namespace rosflight {

typedef struct
{
  int16_t channel;
  int16_t direction;
} rc_switch_t;

typedef struct
{
    param_id_t channel_param;
    param_id_t max_angle_param;
    param_id_t max_rate_param;
    param_id_t center_param;
    param_id_t bottom_param;
    param_id_t range_param;
    control_channel_t* control_channel_ptr;
} rc_channel_t;

typedef enum
{
    RC_X,
    RC_Y,
    RC_Z,
    RC_F
} rc_channel_enum_t;

typedef enum
{
  PARALLEL_PWM,
  CPPM,
} rc_type_t;


extern bool _calibrate_rc;
void init_rc(void);
bool rc_switch(int16_t channel);
bool receive_rc();
bool rc_low(int16_t channel);
bool rc_high(int16_t channel);

}

#endif
