/*
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2017, James Jackson and Daniel Koch, BYU MAGICC Lab, Provo UT
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdbool.h>
#include "mux.h"


namespace rosflight
{

void Mux::init(Arming_FSM *_fsm, Params *_params, Board *_board)
{
  fsm = _fsm;
  params = _params;
  board = _board;
  _new_command = false;
}

void Mux::do_muxing(uint8_t mux_channel)
{
  mux_t *mux_ptr = &(muxes[mux_channel]);
  if (mux_ptr->rc->active)
  {
    (*mux_ptr->combined) = (*mux_ptr->rc);
  }
  else if (mux_ptr->onboard->active)
  {
    (*mux_ptr->combined) = (*mux_ptr->onboard);
  }
  else
  {
    // Default to RC if neither is active
    (*mux_ptr->combined) = (*mux_ptr->rc);
    mux_ptr->combined->active = true;
  }
}

void Mux::do_min_throttle_muxing()
{
  if (_offboard_control.F.active)
  {
    if (_rc_control.F.type == THROTTLE && _offboard_control.F.type == THROTTLE)
    {
      _combined_control.F.value = (_rc_control.F.value > _offboard_control.F.value) ?
                                  _offboard_control.F.value : _rc_control.F.value;
      _combined_control.F.type = THROTTLE;
      _combined_control.F.active = true;
    }
    else
    {
      // I'm still not quite sure how to handle the mixed altitude/throttle cases
      // for now, just pass the rc along.  I expect that what we really need to do
      // is run the altitude controller here so we can compare throttle to throttle
      _combined_control.F = _rc_control.F;
    }
  }
}

bool Mux::mux_inputs()
{
  if (!_new_command)
  {
    // we haven't received any new commands, so we shouldn't do anything
    return false;
  }
  // otherwise combine the new commands

//  if (fsm->_armed_state == DISARMED_FAILSAFE || fsm->_armed_state == ARMED_FAILSAFE)
//  {
//    _combined_control = _failsafe_control;
//  }

//  else
//  {
    for (uint8_t i = 0; i < 4; i++)
    {
      if (i == MUX_F && params->get_param_int(PARAM_RC_OVERRIDE_TAKE_MIN_THROTTLE))
      {
        do_min_throttle_muxing();
      }
      else
      {
        do_muxing(i);
      }
    }

    // Light to indicate override
    if (_rc_control.x.active || _rc_control.y.active || _rc_control.z.active || _rc_control.F.active)
    {
      board->led0_on();
    }
    else
    {
      board->led0_off();
    }
//  }

  // reset the new command flag
  _new_command = false;
  return true;
}

}
