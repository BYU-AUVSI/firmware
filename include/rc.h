#ifndef RC_H_
#define RC_H_

#include <stdint.h>
#include <stdbool.h>

#include "mux.h"
#include "param.h"
#include "board.h"
#include "arming_fsm.h"
#include "common.h"

namespace rosflight {

class Arming_FSM;
class Params;
class Mux;

class RC
{

public:
  typedef enum
  {
    PARALLEL_PWM,
    CPPM,
  } rc_type_t;


  typedef enum
  {
    RC_X,
    RC_Y,
    RC_Z,
    RC_F
  } rc_channel_enum_t;


  bool _calibrate_rc;
  void init_rc(Arming_FSM* _fsm, Board* _board, Params* _params, Mux* _mux);
  bool rc_switch(int16_t channel);
  bool receive_rc();
  bool rc_low(int16_t channel);
  bool rc_high(int16_t channel);


private:
  typedef struct
  {
    int16_t channel;
    int16_t direction;
  } rc_switch_t;

  typedef struct
  {
    uint16_t channel_param;
    uint16_t max_angle_param;
    uint16_t max_rate_param;
    uint16_t center_param;
    uint16_t bottom_param;
    uint16_t range_param;
    control_channel_t* control_channel_ptr;
  } rc_channel_t;

  Arming_FSM* fsm;
  Board* board;
  Params* params;
  Mux* mux;

  uint32_t time_of_last_stick_deviation = 0;
  uint32_t last_rc_receive_time = 0;

  rc_channel_t channels[4];
  rc_switch_t switches[4];

  void calibrate_rc();
  void init_switches();
  void interpret_command_values();
  void interpret_command_type();
  bool sticks_deviated(uint32_t now_ms);
};
}
#endif
