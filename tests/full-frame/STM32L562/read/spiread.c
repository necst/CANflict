#include "main.h"

#include <platforms/STM32/platform.h>
#include <senders/SPISender.h>


#define LED_PORT GPIOG
#define LED_PIN GPIO_PIN_12
#define CAN_RX_PORT GPIOB
#define CAN_RX_PIN GPIO_PIN_4

#define SAMPLING_POINT 0
#define N_SAMPLES 7

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

  struct CANflict_ReceiverConf_t conf;
  conf.spi = &spi;

  // The SPI speed is 7 times faster than the baudrate, but the
  // prescaler must be a power of two, so we have to oversample
  uint8_t rcvBuf[((MSG_BIT_LEN * N_SAMPLES) / 8) + 1];

  CANflict_receiver_init(&conf);

  while (1) {
    // Wait for falling edges
    while (HAL_GPIO_ReadPin(CAN_RX_PORT, CAN_RX_PIN))
      ;

    // Read bits until ACK
    CANflict_receiver_rcvBits(&conf, rcvBuf, MSG_BIT_LEN * N_SAMPLES);

    // Check received bits
    int rcvBits = checkMsg(rcvBuf, (MSG_BIT_LEN * N_SAMPLES) / 8);

    printf("Received %d\n\r", rcvBits);
  }
}

return 0;
}
