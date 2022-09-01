#pragma once

#include "config.h"
// The peripheral interface that is being implemented
#include "../peripherals/SPI.h"

// Platform-specific headers
#include <IfxQspi_SpiMaster.h>
#include <Ifx_Types.h>

// ------------------- SPI Interface Implementation ---------------------------
struct CANflict_SPI_t {
  Ifx_QSPI *spi;
  IfxQspi_Sclk_Out *clkPin;
  IfxQspi_Mtsr_Out *mosiPin;
  IfxQspi_Mtsr_In *misoPin;
  IfxQspi_Slso_Out *chipSelectPin;
};

static IfxQspi_SpiMaster g_spiMaster;
static IfxQspi_SpiMaster_Channel g_spiMasterChannel;

// Interrupts
IFX_INTERRUPT(masterTxISR, 0, ISR_PRIORITY_SPI_TX);
IFX_INTERRUPT(masterRxISR, 0, ISR_PRIORITY_SPI_RX);
IFX_INTERRUPT(masterErISR, 0, ISR_PRIORITY_SPI_ER);

void masterTxISR() {
  IfxCpu_enableInterrupts();
  IfxQspi_SpiMaster_isrTransmit(&g_spiMaster);
}

void masterRxISR() {
  IfxCpu_enableInterrupts();
  IfxQspi_SpiMaster_isrReceive(&g_spiMaster);
}

void masterErISR() {
  IfxCpu_enableInterrupts();
  IfxQspi_SpiMaster_isrError(&g_spiMaster);
}

static inline bool CANflict_SPI_init__internal(struct CANflict_SPI_t *spi,
                                               uint32_t baudrateHz) {
  if (spi == NULL || spi->spi == NULL)
    return false;

  // ----------- Init SPI module
  // Initialize Config
  IfxQspi_SpiMaster_Config spiMasterConfig;
  IfxQspi_SpiMaster_initModuleConfig(&spiMasterConfig, spi->spi);
  spiMasterConfig.base.mode = SpiIf_Mode_master;
  // Select Pins
  // clang-format off
  const IfxQspi_SpiMaster_Pins pins = {spi->clkPin, IfxPort_OutputMode_pushPull,
                                       spi->mosiPin, IfxPort_OutputMode_pushPull,
                                       spi->misoPin, IfxPort_InputMode_pullDown,
                                       IfxPort_PadDriver_cmosAutomotiveSpeed3};
  // clang-format on
  spiMasterConfig.pins = &pins;
  // Set the ISR priorities and the service provider
  spiMasterConfig.base.txPriority = ISR_PRIORITY_SPI_TX;
  spiMasterConfig.base.rxPriority = ISR_PRIORITY_SPI_RX;
  spiMasterConfig.base.erPriority = ISR_PRIORITY_SPI_ER;
  spiMasterConfig.base.isrProvider = ISR_PROVIDER_SPI;
  // Initialize the QSPI module
  IfxQspi_SpiMaster_initModule(&g_spiMaster, &spiMasterConfig);

  // ----------- Init Master Channel
  // Initialize Config
  IfxQspi_SpiMaster_ChannelConfig spiMasterChannelConfig;
  IfxQspi_SpiMaster_initChannelConfig(&spiMasterChannelConfig, &g_spiMaster);
  // Baudrate
  if (baudrateHz > 0)
    spiMasterChannelConfig.base.baudrate = baudrateHz;
  // Select the port pin for the Chip Select signal
  const IfxQspi_SpiMaster_Output csPin = {
      spi->chipSelectPin, IfxPort_OutputMode_pushPull,
      IfxPort_PadDriver_cmosAutomotiveSpeed1};
  spiMasterChannelConfig.sls.output = csPin;
  // Initialize the QSPI Master channel
  IfxQspi_SpiMaster_initChannel(&g_spiMasterChannel, &spiMasterChannelConfig);

  return true;
}

static inline bool CANflict_SPI_init(struct CANflict_SPI_t *spi) {
  if (spi == NULL || spi->spi == NULL)
    return false;
  return CANflict_SPI_init__internal(spi, 0U);
}

static inline bool CANflict_SPI_setBaudrate(struct CANflict_SPI_t *spi,
                                            uint32_t baudrateHz) {
  if (spi == NULL || spi->spi == NULL)
    return false;
  return CANflict_SPI_init__internal(spi, baudrateHz);
}

static inline bool CANflict_SPI_deinit(struct CANflict_SPI_t *spi) {
  if (spi == NULL || spi->spi == NULL)
    return false;
  // Nothing to do here
  return true;
}

static inline uint32_t CANflict_SPI_write(struct CANflict_SPI_t *spi,
                                          uint8_t *buf, uint32_t byteLen) {
  if (buf == NULL || spi->spi != g_spiMaster.qspi)
    return 0U;

  while (IfxQspi_SpiMaster_getStatus(&g_spiMasterChannel) ==
         SpiIf_Status_busy) { /* Wait until the previous communication has
                                 finished, if any */
  }

  IfxQspi_SpiMaster_exchange(&g_spiMasterChannel, buf, NULL_PTR, byteLen);
  return byteLen;
}

static inline uint32_t CANflict_SPI_read(struct CANflict_SPI_t *spi,
                                         uint8_t *buf, uint32_t byteLen) {
  if (buf == NULL || spi->spi != g_spiMaster.qspi)
    return 0U;

  while (IfxQspi_SpiMaster_getStatus(&g_spiMasterChannel) ==
         SpiIf_Status_busy) { /* Wait until the previous communication has
                                 finished, if any */
  }

  IfxQspi_SpiMaster_exchange(&g_spiMasterChannel, NULL_PTR, buf, byteLen);
  return byteLen;
}
