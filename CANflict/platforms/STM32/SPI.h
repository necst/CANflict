#pragma once

// The peripheral interface that is being implemented
#include "../peripherals/SPI.h"

// Platform-specific header
#include "stm32l5xx_hal.h"

// ------------------- SPI Interface Implementation ---------------------------

struct CANflict_SPI_t {
  SPI_HandleTypeDef *spi;
  SPI_TypeDef *instance;
  uint32_t prescaler;
};

static inline bool CANflict_SPI_init(struct CANflict_SPI_t *spi) {
  if (spi == NULL || spi->spi == NULL)
    return false;

  spi->spi->Instance = spi->instance;
  spi->spi->Init.Mode = SPI_MODE_MASTER;
  spi->spi->Init.Direction = SPI_DIRECTION_2LINES;
  spi->spi->Init.DataSize = SPI_DATASIZE_8BIT;
  spi->spi->Init.CLKPolarity = SPI_POLARITY_LOW;
  spi->spi->Init.CLKPhase = SPI_PHASE_1EDGE;
  spi->spi->Init.NSS = SPI_NSS_SOFT;
  spi->spi->Init.BaudRatePrescaler = spi->prescaler;
  spi->spi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  spi->spi->Init.TIMode = SPI_TIMODE_DISABLE;
  spi->spi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  spi->spi->Init.CRCPolynomial = 7;
  spi->spi->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  spi->spi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(spi->spi) != HAL_OK) {
    Error_Handler();
  }

  return true;
}

static inline bool CANflict_SPI_deinit(struct CANflict_SPI_t *spi) {
  if (spi == NULL || spi->spi == NULL)
    return false;

  HAL_SPI_DeInit(spi->spi);
  return true;
}

static inline bool CANflict_SPI_setBaudrate(struct CANflict_SPI_t *spi,
                                            uint32_t baudrateHz) {
  if (spi == NULL || spi->spi == NULL)
    return false;

  // Not implemented, use the prescaler instead
  return false;
}

static inline uint32_t CANflict_SPI_write(struct CANflict_SPI_t *spi,
                                          uint8_t *buf, uint32_t byteLen) {
  if (buf == NULL || spi == NULL)
    return 0U;
  // TODO: might be useful to implement also a non-blocking version
  volatile uint8_t *dataReg = &spi->spi->Instance->DR;

  __HAL_SPI_ENABLE(spi->spi);

  for (uint32_t i = 0; i < byteLen; ++i) {
    while (__HAL_SPI_GET_FLAG(spi->spi, SPI_FLAG_TXE) == 0)
      ;
    *dataReg = buf[i];
  }
  while (__HAL_SPI_GET_FLAG(spi->spi, SPI_FLAG_TXE) == 0)
    ;
  *(volatile uint8_t *)dataReg = 0xffff;
  while (__HAL_SPI_GET_FLAG(spi->spi, SPI_FLAG_TXE) == 0)
    ;

  for (int i = 0; i < 100; i++)
    ;

  __HAL_SPI_DISABLE(spi->spi);

  return byteLen;
}

static inline uint32_t CANflict_SPI_read(struct CANflict_SPI_t *spi,
                                         uint8_t *buf, uint32_t byteLen) {
  if (buf == NULL || spi == NULL)
    return 0U;

  SET_BIT(spi->spi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
  __HAL_SPI_ENABLE(spi->spi);

  volatile uint8_t *dataReg = &spi->spi->Instance->DR;
  for (unsigned i = 0; i < byteLen; ++i) {
    *dataReg = 0;

    while (__HAL_SPI_GET_FLAG(spi->spi, SPI_FLAG_RXNE) == 0)
      ;
    buf[i] = *dataReg;
  }

  __HAL_SPI_DISABLE(spi->spi);

  return byteLen;
}
