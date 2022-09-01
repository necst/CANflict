/*
 * \brief Example of how to use the GPIOReceiver_poll on the LPC11C24 board.
 *
 * To conduct this experiment, the LPC11C24 board has been wired to an MCP2551
 * CAN transceiver as follows:
 *      - Pin 1 of the MCP2551 to the board's PIO2_0
 *      - Pin 2 of the MCP2551 to the board's GND
 *      - Pin 3 of the MCP2551 to an external 5V source
 *      - Pin 4 of the MCP2551 to the board's PIO0_9
 *      - Pin 8 of the MCP2551 to ground with a 10K resistor
 *      - Pin 6 and 7 of the MCP2551 to CANL and CANH, respectively
 * A sender board was attached to the bus, and its baudrate was set at 200 kHz.
 */

// Target headers
#include <board.h>

// CANflict headers
#include <platforms/NXP/platform.h>
#include <receivers/GPIOReceiver_poll.h>

#define LED LPC_GPIO, 0, 7
#define CAN_RX LPC_GPIO, 2, 0
#define CAN_TX LPC_GPIO, 2, 1

#define MSG_BIT_LEN 103
static const bool EXPECTED_MSG[MSG_BIT_LEN] = {
    0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
    1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1,
    0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0,
    1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1}; //, /*ACK:*/ 1};

static inline void initLed(void) {
  Chip_GPIO_SetPinDIROutput(LED);
  Chip_GPIO_SetPinState(LED, true);
}

static inline void sendAck(void) {
  for (int i = 0; i < 200; i++)
    ;
  Chip_GPIO_SetPinState(CAN_TX, false);
  for (int i = 0; i < 290; i++)
    ;
  Chip_GPIO_SetPinState(CAN_TX, true);
}

void main(void) {
  // Init board
  SystemCoreClockUpdate();
  Board_Init();

  initLed();

  // CANTX pin for sending the ACK
  struct CANflict_GPIO_t tx_gpio;
  tx_gpio.gpioPeriph = LPC_GPIO;
  tx_gpio.port = 2;
  tx_gpio.pin = 1;
  CANflict_GPIO_init(&tx_gpio, false);

  // CANflict Setup
  struct CANflict_GPIO_t rx_gpio;
  rx_gpio.gpioPeriph = LPC_GPIO;
  rx_gpio.port = 2;
  rx_gpio.pin = 0;

  struct CANflict_ReceiverConf_t rcvConf;
  rcvConf.gpio = &rx_gpio;
  rcvConf.initialCycles = 30;
  rcvConf.cyclesBetweenBits = 118;

  bool rcvBuf[MSG_BIT_LEN];

  CANflict_receiver_init(&rcvConf);
  int n = 0;
  while (1) {
    // Wait for falling edges
    while (Chip_GPIO_GetPinState(CAN_RX) == true)
      ;

    // Read bits until ACK
    CANflict_receiver_rcvBits(&rcvConf, rcvBuf, MSG_BIT_LEN);
    sendAck();

    // Check received bits
    int i = 0;
    for (i = 0; i < MSG_BIT_LEN; i++) {
      if ((rcvBuf[i] & 0x1) != (EXPECTED_MSG[i] & 0x1)) {
        Chip_GPIO_SetPinState(LED, (n % 2));
        break;
      } else {
        ++n;
      }
    }
  }
}
