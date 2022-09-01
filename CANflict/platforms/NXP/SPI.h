#pragma once

// The peripheral interface that is being implemented
#include "../peripherals/SPI.h"

// Platform-specific header
#include <board.h>

// ------------------- SPI Interface Implementation ---------------------------

struct CANflict_SPI_t {
  const LPC_SSP_T *spi;
};

/* Set pin mux for SSP operation */
static void CANflict_SPI_InitPins(void) {
#if (defined(BOARD_NXP_XPRESSO_11U14) || defined(BOARD_NGX_BLUEBOARD_11U24))
  /* Only SSP0 is supported */
  Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 2,
                       (IOCON_FUNC1 | IOCON_MODE_PULLUP)); /* SSEL0 */
  Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 29,
                       (IOCON_FUNC1 | IOCON_MODE_PULLUP)); /* SCK0 */
  Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 8,
                       (IOCON_FUNC1 | IOCON_MODE_PULLUP)); /* MISO0 */
  Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9,
                       (IOCON_FUNC1 | IOCON_MODE_PULLUP)); /* MOSI0 */
#elif defined(BOARD_NXP_XPRESSO_11C24)
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8,
                       (IOCON_FUNC1 | IOCON_MODE_INACT)); /* MISO0 */
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9,
                       (IOCON_FUNC1 | IOCON_MODE_INACT)); /* MOSI0 */
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2,
                       (IOCON_FUNC1 | IOCON_MODE_INACT)); /* SSEL0 */
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_11,
                       (IOCON_FUNC1 | IOCON_MODE_INACT)); /* SCK0 */
  Chip_IOCON_PinLocSel(LPC_IOCON, IOCON_SCKLOC_PIO2_11);
#elif defined(BOARD_MCORE48_1125)
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8,
                       (IOCON_FUNC1 | IOCON_MODE_INACT)); /* MISO0 */
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9,
                       (IOCON_FUNC1 | IOCON_MODE_INACT)); /* MOSI0 */
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2,
                       (IOCON_FUNC1 | IOCON_MODE_INACT)); /* SSEL0 */
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_6,
                       (IOCON_FUNC2 | IOCON_MODE_INACT)); /* SCK0 */
  Chip_IOCON_PinLocSel(LPC_IOCON, IOCON_SCKLOC_PIO0_6);
#else
#error "SSP Pin muxing not configured"
#endif
}

static inline bool CANflict_SPI_init(struct CANflict_SPI_t *spi) {
  if (spi == NULL || spi->spi == NULL)
    return false;

  CANflict_SPI_InitPins();
  Chip_SSP_Init(spi->spi);

  SSP_ConfigFormat ssp_format;
  ssp_format.frameFormat = CHIP_SSP_FRAME_FORMAT_TI; // SSP_FRAMEFORMAT_SPI;
  ssp_format.bits = SSP_BITS_8;
  ssp_format.clockMode = SSP_CLOCK_MODE0;
  Chip_SSP_SetFormat(spi->spi, ssp_format.bits, ssp_format.frameFormat,
                     ssp_format.clockMode);
  Chip_SSP_SetMaster(spi->spi, /*master=*/1);
  Chip_SSP_Enable(spi->spi);

  return true;
}

static inline bool CANflict_SPI_deinit(struct CANflict_SPI_t *spi) {
  if (spi == NULL || spi->spi == NULL)
    return false;

  Chip_SSP_DeInit(spi->spi);
  return true;
}

static inline bool CANflict_SPI_setBaudrate(struct CANflict_SPI_t *spi,
                                            uint32_t baudrateHz) {
  if (spi == NULL || spi->spi == NULL)
    return false;

  // TODO: check maximum bitrate

  Chip_SSP_SetBitRate(spi->spi, baudrateHz);
  return true;
}

static inline uint32_t CANflict_SPI_write(struct CANflict_SPI_t *spi,
                                          uint8_t *buf, uint32_t byteLen) {
  if (buf == NULL || spi == NULL)
    return 0U;
  // TODO: might be useful to implement also a non-blocking version
  // Chip_SSP_RWFrames_Blocking(spi->spi, &xf_setup);

  for (unsigned i = 0; i < byteLen; ++i) {
    while (!Chip_SSP_GetStatus(spi->spi, SSP_STAT_TFE))
      ; // Wait until fifo empty
    Chip_SSP_SendFrame(spi->spi, buf[i]);
  }

  return byteLen;
}

static inline uint32_t CANflict_SPI_read(struct CANflict_SPI_t *spi,
                                         uint8_t *buf, uint32_t byteLen) {
  if (buf == NULL || spi == NULL)
    return 0U;

  for (unsigned i = 0; i < byteLen; ++i) {
    // Wait until fifo empty
    while (!Chip_SSP_GetStatus(spi->spi, SSP_STAT_TFE))
      ;
    // Send a byte
    Chip_SSP_SendFrame(spi->spi, 0xFF);
    // Receive a byte
    Chip_SSP_ReceiveFrame(spi->spi);
  }

  return byteLen;
}

static inline uint32_t CANflict_SPI_fastRead(struct CANflict_SPI_t *spi,
                                             uint8_t *buf, uint32_t byteLen) {
  for (unsigned i = 0; i < byteLen; ++i) {
    // Send a byte
    Chip_SSP_SendFrame(spi->spi, 0xFF);
    // Receive a byte
    Chip_SSP_ReceiveFrame(spi->spi);
  }

  return byteLen;
}
