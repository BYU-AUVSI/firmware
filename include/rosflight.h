#ifndef ROSFLIGHT_H_
#define ROSFLIGHT_H_

#include "board.h"
#include "commlink.h"
#include "param.h"
#include "sensors.h"
#include "arming_fsm.h"
#include "estimator.h"
#include "rc.h"
#include "controller.h"

namespace rosflight {

class ROSflight
{

private:
  Board* board_;
  CommLink* commlink_;

  Params params_;
  Arming_FSM fsm_;
  Sensors sensors_;
  Estimator estimator_;
  Mux mux_;
  Mixer mixer_;
  Controller controller_;
  RC rc_;


public:
  ROSflight(Board* _board, CommLink* _commlink);

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
