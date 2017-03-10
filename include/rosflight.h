#ifndef ROSFLIGHT_H_
#define ROSFLIGHT_H_

#include "board.h"
#include "mavlink.h"
#include "param.h"
#include "sensors.h"
#include "arming_fsm.h"

namespace rosflight {

class ROSflight
{

private:
  Board* board_;
  Mavlink* mavlink_;

  Params params_;
  Arming_FSM fsm_;
  Sensors sensors_;
//  Mux mux;
//  Mixer mixer;
//  Controller controller;
//  RC rc;
//  Mode mode;


public:
  ROSflight(Board* _board, Mavlink* _mavlink);

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
