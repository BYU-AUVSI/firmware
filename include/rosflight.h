#ifndef ROSFLIGHT_H_
#define ROSFLIGHT_H_

#include "board.h"
#include "param.h"

namespace rosflight {

class ROSflight
{

private:
  Board* board_;
  Params* params_;

public:
  ROSflight(Board* _board, Params* _params);

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
