/*
 * \brief Example of how to use the I2CSender on the LPC11C24 board.
 *
 * To conduct this experiment, the LPC11C24 board has been wired to an MCP2551
 * CAN transceiver as follows:
 *      - Pin 1 of the MCP2551 to the board's PIO0_5 (I2C_SDA)
 *      - Pin 2 of the MCP2551 to the board's GND
 *      - Pin 3 of the MCP2551 to an external 5V source
 *      - Pin 4 of the MCP2551 to the board's PIO0_8
 *      - Pin 8 of the MCP2551 to ground with a 10K resistor
 *      - Pin 6 and 7 of the MCP2551 to CANL and CANH, respectively
 */

#include "board.h"

#include <platforms/NXP/platform.h>
#include <senders/I2CSender.h>

#define BYTE_LEN 8
#define CAN_BAUDRATE 210000 // = 200 KBits/s
static const bool MSG_BITS[] = {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0,         1, 1,
                                0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0,         1, 1,
                                0, 0, 0, 1, 0, 1, 0, 1, 1, 1, /*ACK*/ 0, 1, 1};
static const uint32_t MSG_BIT_LEN = sizeof(MSG_BITS);

int main(void) {
  // Init board
  SystemCoreClockUpdate();
  Board_Init();

  // CANflict Setup
  struct CANflict_I2C_t i2c;
  i2c.id = I2C0;
  i2c.i2c = LPC_I2C;

  struct CANflict_SenderConf_t conf;
  conf.I2C = &i2c;
  conf.freqHz = CAN_BAUDRATE;
  conf.startCondDuration = 1;
  conf.stopCondDuration = 1;
  conf.interFrameDuration = 2;

  CANflict_sender_init(&conf);
  CANflict_sender_setMsgToSend(&conf, MSG_BITS, MSG_BIT_LEN);

  // Send continuously with CANflict
  while (1) {
    for (int i = 0; i < 1000000; ++i)
      /* busy wait between messages */;
    CANflict_sender_sendMsg(&conf);
  }

  return 0;
}
