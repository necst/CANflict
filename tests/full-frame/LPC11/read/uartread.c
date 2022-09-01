/*
 * \brief Example of how to use the UARTSender on the LPC11C24 board.
 *
 * To conduct this experiment, the LPC11C24 board has been wired to an MCP2551
 * CAN transceiver as follows:
 *      - Pin 1 of the MCP2551 to the board's PIO1_6 (UART1_RX)
 *      - Pin 2 of the MCP2551 to the board's GND
 *      - Pin 3 of the MCP2551 to an external 5V source
 *      - Pin 4 of the MCP2551 to the board's PIO0_9
 *      - Pin 8 of the MCP2551 to ground with a 10K resistor
 *      - Pin 6 and 7 of the MCP2551 to CANL and CANH, respectively
 */

#include "board.h"
#include "chip.h"
#include <platforms/NXP/platform.h>
#include <receivers/UARTReceiver.h>


static const uint8_t MSG_BITS[] = {0x77, 0x47, 0xd0, 0x1a, 0x34,
                                   0x35, 0x8,  0xb1, 0x4b};
static const uint32_t MSG_BIT_LEN = sizeof(MSG_BITS) * 8;
static const uint8_t MSG_BYTE_LEN = sizeof(MSG_BITS);

#define BAUDRATE 1000000 // = 1 MBit/s

int main(void) {
  // Init board
  SystemCoreClockUpdate();
  Board_Init();

  // CANflict Setup
  struct CANflict_UART_t uart;
  uart.uart = LPC_USART;

  struct CANflict_ReceiverConf_t conf;
  conf.uart = &uart;
  conf.freqHz = BAUDRATE;
  conf.wordLen = 8;
  conf.stopBits = 1;

  uint8_t rcvBuf[MSG_BYTE_LEN];

  CANflict_receiver_init(&conf);
  int correct = 0;
  while (1) {
    // Read bits
    CANflict_receiver_rcvBits(&conf, rcvBuf, MSG_BIT_LEN);

    // Check received bits
    n = 0;
    for (int i = 0; i < MSG_BYTE_LEN; i++) {
      if (rcvBuf[i] != EXPECTED_MSG[i]) {
        Chip_GPIO_SetPinToggle(LED);
        break;
      } else {
        ++n;
      }
    }
  }
}
