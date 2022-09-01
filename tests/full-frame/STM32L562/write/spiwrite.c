#include "main.h"

#include <platforms/STM32/platform.h>
#include <senders/SPISender.h>


#define BYTE_LEN 8

static const bool MSG_BITS[] = {
    0, 1, 1, 1, 0, 1, 1, 1, 0,          1, 1, 1, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 0, 1, 1, 0, 1, 0,          1, 1, 0, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 1, 0, 1, 0, 1, 1,          0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 1, 0, 1, 1,          1, 0, 1, 0, 0, 1, 0, 1, 1, 1,
    1, 0, 1, 0, 1, 1, 1, 0, 1,          0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    0, 0, 1, 0, 0, 0, 0, 1, /*ACK:*/ 1, 1, 1, 1, 1, 1, 1, 1, 1};
#define MSG_BIT_LEN sizeof(MSG_BITS)

SPI_HandleTypeDef hspi1;

int main(void) {
  // Init board
  SystemClock_Config();
  MX_ICACHE_Init();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();

  // CANflict Setup
  struct CANflict_SPI_t spi;
  spi.spi = &hspi1;
  spi.instance = SPI1;
  spi.prescaler = SPI_BAUDRATEPRESCALER_16;

  struct CANflict_SenderConf_t conf;
  conf.spi = &spi;
  // The system clock speed is 110MHz and the presc has to be set to a power of
  // two, so oversampling is necessary
  conf.nSamples = 7;

  CANflict_sender_init(&conf);
  CANflict_sender_setMsgToSend(&conf, MSG_BITS, MSG_BIT_LEN);

  // Send continuously with CANflict
  while (1) {
    /* busy wait between messages */
    for (int i = 0; i < 1000000; ++i)
      ;
    CANflict_sender_sendMsg(&conf);
  }

  return 0;
}
