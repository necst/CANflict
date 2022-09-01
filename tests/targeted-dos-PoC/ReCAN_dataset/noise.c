#include <stdint.h>
#include <stdio.h>

#include "noise.h"

int main() {
  Msg next;
  uint64_t i;

  for (i = 0; i < N_MSG; i++) {
    next = noise[i];
    sleepUs(next.delay_us);
    sendCANMsg(next.ID, next.payload);
  }

  blinkLed();
  PRINTOUT("Finished %d\n", i);

  while (1)
    ;
}