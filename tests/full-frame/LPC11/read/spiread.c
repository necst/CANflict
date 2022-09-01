/*
 * \brief Example of how to use the SPIReceiver on the LPC11C24 board.
 *
 * To conduct this experiment, the LPC11C24 board has been wired to an MCP2551
 * CAN transceiver as follows:
 *      - Pin 1 of the MCP2551 to the board's PIO0_9 (SPI1_MOSI)
 *      - Pin 2 of the MCP2551 to the board's GND
 *      - Pin 3 of the MCP2551 to an external 5V source
 *      - Pin 4 of the MCP2551 to the board's PIO0_8 (SPI1_MISO)
 *      - Pin 8 of the MCP2551 to ground with a 10K resistor
 *      - Pin 6 and 7 of the MCP2551 to CANL and CANH, respectively
 */

#include "board.h"

#include <platforms/NXP/platform.h>
#include <receivers/SPIReceiver.h>

#define LED LPC_GPIO, 0, 7
#define CAN_RX LPC_GPIO, 0, 8

#define BYTE_LEN 8
#define CAN_BAUDRATE 500000 // = 500 kBits/s
static const uint8_t EXPECTED_MSG[] = {0x77, 0x47, 0xd0, 0x1a, 0x34, 0x35,
                                       0x8,  0xb1, 0x4b, 0xaf, 0xb,  0x22};
#define MSG_BIT_LEN sizeof(EXPECTED_MSG) * BYTE_LEN
#define MSG_BYTE_LEN sizeof(EXPECTED_MSG)

#define GPIO_PININT 8             /* GPIO pin number mapped to PININT */
#define GPIO_PININT_PORT 0        /* GPIO port number mapped to PININT */
#define IOCON_PIN_ID IOCON_PIO0_8 /* IOCON pin identifer */
#define PININT_IRQ_HANDLER                                                     \
  PIOINT0_IRQHandler                /* GPIO interrupt IRQ function name */
#define PININT_NVIC_NAME EINT0_IRQn /* GPIO interrupt NVIC interrupt name */

uint8_t rcvBuf[MSG_BYTE_LEN + 4];
struct CANflict_SPI_t spi;

/// HACK: executing the whole read operation inside the IRQ routine minimizes
/// the delay between the SoF recognition and the first bit read.
void PININT_IRQ_HANDLER(void) {
  CANflict_SPI_fastRead(&spi, rcvBuf, MSG_BYTE_LEN + 4);
  /* Clear interrupt */
  Chip_GPIO_ClearInts(LPC_GPIO, GPIO_PININT_PORT, (1 << GPIO_PININT));
}

static inline void initLed(void) {
  Chip_GPIO_SetPinDIROutput(LED);
  Chip_GPIO_SetPinState(LED, true);
}

int main(void) {
  // Init board
  SystemCoreClockUpdate();
  Board_Init();

  initLed();

  /* Configure channel interrupt as edge sensitive and falling edge interrupt */
  Chip_GPIO_SetupPinInt(LPC_GPIO, GPIO_PININT_PORT, GPIO_PININT,
                        GPIO_INT_FALLING_EDGE);
  /* Enable GPIO pin interrupt */
  Chip_GPIO_EnableInt(LPC_GPIO, GPIO_PININT_PORT, (1 << GPIO_PININT));
  /* Enable interrupt in the NVIC */
  NVIC_EnableIRQ(PININT_NVIC_NAME);

  // CANflict Setup

  spi.spi = LPC_SSP0;

  struct CANflict_ReceiverConf_t conf;
  conf.spi = &spi;
  conf.freqHz = CAN_BAUDRATE;

  CANflict_receiver_init(&conf);

  // The received bits are saved in a byte array by the CAN receiver

  int n = 0;
  while (1) {
    __WFI();

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

    //    printf("Received %d\n\r", n_correct);
  }
}
