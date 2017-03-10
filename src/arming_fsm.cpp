#include <stdint.h>
#include <stdbool.h>

#include "arming_fsm.h"

namespace rosflight
{

Arming_FSM::Arming_FSM()
{
  _armed_state = DISARMED;
  prev_time_ms = 0;
  time_sticks_have_been_in_arming_position_ms = 0;
}

void Arming_FSM::init_mode(Board *_board, Sensors *_sensors, Params *_params, RC *_rc)
{
   rc_ = _rc;
   board_ = _board;
   sensors_ = _sensors;
   params_ = _params;
  _armed_state = DISARMED;
}

bool Arming_FSM::arm(void)
{
  started_gyro_calibration = false;
  if (!started_gyro_calibration && _armed_state & DISARMED)
  {
    sensors_->start_gyro_calibration();
    started_gyro_calibration = true;
    return false;
  }
  else if (sensors_->gyro_calibration_complete())
  {
    started_gyro_calibration = false;
    _armed_state = ARMED;
    board_->led1_on();
    return true;
  }
  return false;
}

void Arming_FSM::disarm(void)
{
  _armed_state = DISARMED;
  board_->led1_off();
}

bool Arming_FSM::check_failsafe(void)
{
  if (board_->pwm_lost())
  {
    // Set the FAILSAFE bit
    _armed_state = (ARMED) ? ARMED_FAILSAFE : DISARMED_FAILSAFE;
    return true;
  }

  else
  {
    for (int8_t i = 0; i<params_->get_param_int(PARAM_RC_NUM_CHANNELS); i++)
    {
      if(board_->pwm_read(i) < 900 || board_->pwm_read(i) > 2100)
      {
        _armed_state = (ARMED) ? ARMED_FAILSAFE : DISARMED_FAILSAFE;

        // blink LED
        static uint8_t count = 0;
        if (count > 25)
        {
          board_->led1_toggle();
          count = 0;
        }
        count++;
        return true;
      }
    }

    // we got a valid RC measurement for all channels and pwm is active
    // Clear the FAILSAFE bit
    _armed_state = (ARMED_FAILSAFE) ? ARMED : DISARMED;
    return false;
  }
}


bool Arming_FSM::check_mode()
{
  uint32_t now = board_->clock_millis();

  // see it has been at least 20 ms
  uint32_t dt = now-prev_time_ms;
  if (dt < 20)
  {
    return false;
  }

  // if it has, then do stuff
  prev_time_ms = now;

  // check for failsafe mode
  if (check_failsafe())
  {
    return true;
  }
  else
  {
    // check for arming switch
    if (params_->get_param_int(PARAM_ARM_STICKS))
    {
      if (_armed_state == DISARMED)
      {
        // if left stick is down and to the right
        if (rc_->rc_low(RC::RC_F) && rc_->rc_high(RC::RC_Z))
        {
          time_sticks_have_been_in_arming_position_ms += dt;
        }
        else
        {
          time_sticks_have_been_in_arming_position_ms = 0;
        }
        if (time_sticks_have_been_in_arming_position_ms > 500)
        {
          if (arm())
            time_sticks_have_been_in_arming_position_ms = 0;
        }
      }
      else // _armed_state is ARMED
      {
        // if left stick is down and to the left
        if (rc_->rc_low(RC::RC_F) && rc_->rc_low(RC::RC_Z))
        {
          time_sticks_have_been_in_arming_position_ms += dt;
        }
        else
        {
          time_sticks_have_been_in_arming_position_ms = 0;
        }
        if (time_sticks_have_been_in_arming_position_ms > 500)
        {
          disarm();
          time_sticks_have_been_in_arming_position_ms = 0;
        }
      }
    }
    else
    {
      if (rc_->rc_switch(params_->get_param_int(PARAM_ARM_CHANNEL)))
      {
        if (_armed_state == DISARMED)
          arm();
      }
      else
      {
        disarm();
      }
    }
  }
  return true;
}

}
