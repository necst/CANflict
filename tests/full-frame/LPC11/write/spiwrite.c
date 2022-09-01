/*
 * \brief Example of how to use the SPISender on the LPC11C24 board.
 *
 * To conduct this experiment, the LPC11C24 board has been wired to an MCP2551
 * CAN transceiver as follows:
 *      - Pin 1 of the MCP2551 to the board's PIO0_9 (SPI1_MOSI)
 *      - Pin 2 of the MCP2551 to the board's GND
 *      - Pin 3 of the MCP2551 to an external 5V source
 *      - Pin 4 of the MCP2551 to the board's PIO0_8
 *      - Pin 8 of the MCP2551 to ground with a 10K resistor
 *      - Pin 6 and 7 of the MCP2551 to CANL and CANH, respectively
 */

#include "board.h"

#include <platforms/NXP/platform.h>
#include <senders/SPISender.h>

#define BYTE_LEN 8
#define N_SAMPLES 1
#define CAN_BAUDRATE 1000000 // = 1 MBits/s
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
  struct CANflict_SPI_t spi;
  spi.spi = LPC_SSP0;

  struct CANflict_SenderConf_t conf;
  conf.spi = &spi;
  conf.freqHz = CAN_BAUDRATE;
  conf.nSamples = N_SAMPLES;

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
