#include "rosflight.h"
#include "naze32.h"
#include "param.h"
#include "arming_fsm.h"

int main(void)
{
  rosflight::Naze32 board;
  rosflight::Mavlink mavlink(&board);

  rosflight::ROSflight firmware(&board, &mavlink);
  firmware.rosflight_init();

  while(1)
  {
    firmware.rosflight_run();
  }
  return 0;
}
