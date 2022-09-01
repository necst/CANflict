/*
 * \brief Example of how to use the GPIOSender_timer on the LPC11C24 board.
 *
 * To conduct this experiment, the LPC11C24 board has been wired to an MCP2551
 * CAN transceiver as follows:
 *      - Pin 1 of the MCP2551 to the board's PIO2_0
 *      - Pin 2 of the MCP2551 to the board's GND
 *      - Pin 3 of the MCP2551 to an external 5V source
 *      - Pin 4 of the MCP2551 to the board's PIO0_9
 *      - Pin 8 of the MCP2551 to ground with a 10K resistor
 *      - Pin 6 and 7 of the MCP2551 to CANL and CANH, respectively
 */

#include "board.h"

#define CANFLICT_LPC11_TIMER CANFLICT_LPC11_TIMER32_0
#include <platforms/NXP/platform.h>

#include <senders/GPIOSender_timer.h>

#define CAN_BAUDRATE 120000 // = 120 kHz

#define MSG_BIT_LEN 112
static const bool MSG_BITS[MSG_BIT_LEN] = {
    0, 1, 1, 1, 0, 1, 1, 1, 0,          1, 1, 1, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 0, 1, 1, 0, 1, 0,          1, 1, 0, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 1, 0, 1, 0, 1, 1,          0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 1, 0, 1, 1,          1, 0, 1, 0, 0, 1, 0, 1, 1, 1,
    1, 0, 1, 0, 1, 1, 1, 0, 1,          0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    0, 0, 1, 0, 0, 0, 0, 1, /*ACK:*/ 1, 1, 1, 1, 1, 1, 1, 1, 1};

int main(void) {
  // Init board
  SystemCoreClockUpdate();
  Board_Init();

  // CANflict Setup
  struct CANflict_GPIO_t tx_gpio;
  tx_gpio.gpioPeriph = LPC_GPIO;
  tx_gpio.port = 2;
  tx_gpio.pin = 0;

  struct CANflict_TIM_t tim;
  tim.timer = LPC_TIMER32_0;

  struct CANflict_SenderConf_t sndConf;
  sndConf.gpio = &tx_gpio;
  sndConf.tim = &tim;
  sndConf.blocking = true;

  const uint32_t sysFreq = Chip_Clock_GetSystemClockRate();
  sndConf.reloadVal = (sysFreq / CAN_BAUDRATE);

  CANflict_sender_init(&sndConf);

  // Send continuously with CANflict
  while (1) {
    for (int i = 0; i < 1000000; ++i)
      /* busy wait between messages */;
    CANflict_sender_setMsgToSend(&sndConf, MSG_BITS, MSG_BIT_LEN);
    CANflict_sender_sendMsg(&sndConf);
  }

  return 0;
}
