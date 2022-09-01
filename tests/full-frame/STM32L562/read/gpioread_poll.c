#include "main.h"

// CANflict headers
#include <platforms/STM32/platform.h>
#include <receivers/GPIOReceiver_poll.h>

#define LED_PORT GPIOG
#define LED_PIN GPIO_PIN_12
#define CAN_RX_PORT GPIOA
#define CAN_RX_PIN GPIO_PIN_1

static const bool EXPECTED_MSG[] = {
    0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
    1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1,
    0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0,
    1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1}; //, /*ACK:*/ 1};
#define MSG_BIT_LEN sizeof(EXPECTED_MSG)

void main(void) {
  // Init board
  SystemCoreClockUpdate();
  Board_Init();

  // CANflict Setup
  struct CANflict_GPIO_t rx_gpio;
  rx_gpio.port = CAN_RX_PORT;
  rx_gpio.pin = CAN_RX_PIN;

  struct CANflict_ReceiverConf_t rcvConf;
  rcvConf.gpio = &rx_gpio;
  rcvConf.initialCycles = 0;
  rcvConf.cyclesBetweenBits = 7; // 500kbit/s

  uint8_t rcvBuf[MSG_BIT_LEN];

  CANflict_receiver_init(&rcvConf);
  int nCorrect = 0;
  while (1) {
    // Wait for falling edges
    while (HAL_GPIO_ReadPin(CAN_RX_PORT, CAN_RX_PIN))
      ;

    // Read bits until ACK
    CANflict_receiver_rcvBits(&rcvConf, rcvBuf, MSG_BIT_LEN);

    // Check received bits
    nCorrect = 0;
    for (int i = 0; i < MSG_BIT_LEN; i++) {
      if ((rcvBuf[i] & 0x1) != (EXPECTED_MSG[i] & 0x1)) {
        HAL_GPIO_TogglePin(CAN_RX_PORT, CAN_RX_PIN);
        break;
      } else {
        ++nCorrect;
      }
    }

    // printf("n correct : %d \n\r", nCorrect);
  }
}
