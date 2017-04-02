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

#ifndef ROSFLIGHT_H_
#define ROSFLIGHT_H_

#include "board.h"
#include "param.h"

#include "sensors.h"
#include "mode.h"
#include "estimator.h"
#include "rc.h"
#include "controller.h"
#include "commlink.h"
#include "mixer.h"

namespace rosflight
{

class CommLink;

class ROSflight
{

private:


public:

  Board *board_;
  CommLink *commlink_;

  Params params_;
  Mode fsm_;
  Sensors sensors_;
  Estimator estimator_;
  Mux mux_;
  Mixer mixer_;
  Controller controller_;
  RC rc_;

  ROSflight(Board *_board, CommLink *_commlink);

  /**
  * @brief Main initialization routine for the ROSflight autopilot flight stack
  */
  void rosflight_init(void);

  /**
  * @brief Main loop for the ROSflight autopilot flight stack
  */
  void rosflight_run(void);

};

}

#endif // ROSFLIGHT_H_
