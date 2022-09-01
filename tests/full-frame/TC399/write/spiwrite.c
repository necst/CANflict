/*
 * \brief Example of how to use the SPISender on the TC399XP TriBoard.
 *
 * To conduct this experiment, the TC399XP board has been wired as follows:
 *      - connected together pin3 (CANL) and pin4 (CANH) of the two CAN
 *        connectors (CAN0 and CAN1)
 *      - connected P20.8 (CAN0TX) to P22.5 (SPI MOSI)
 *
 * The cores were programmed as follows:
 * - cpu0 left idle for interrupt handling
 * - cpu1 programmed to read the CAN bus using the MCMCAN example provided
 *   by Infineon
 * - cpu4 programmed with the following code
 */

// Target headers
#include "Bsp.h"
#include "IfxCan_PinMap.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"


// CANflict headers
#include <platforms/AURIX/platform.h>
#include <senders/SPISender.h>

#define CAN0_TX (&IfxCan_TXD00_P20_8_OUT)
#define MSG_BIT_LEN 112
#define N_SAMPLES 1
#define CAN_BAUDRATE 1000000 // = 1 MBits/s
static const bool MSG_BITS[MSG_BIT_LEN] = {
    0, 1, 1, 1, 0, 1, 1, 1, 0,          1, 1, 1, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 0, 1, 1, 0, 1, 0,          1, 1, 0, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 1, 0, 1, 0, 1, 1,          0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 1, 0, 1, 1,          1, 0, 1, 0, 0, 1, 0, 1, 1, 1,
    1, 0, 1, 0, 1, 1, 1, 0, 1,          0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    0, 0, 1, 0, 0, 0, 0, 1, /*ACK:*/ 1, 1, 1, 1, 1, 1, 1, 1, 1};

extern IfxCpu_syncEvent g_cpuSyncEvent;

void core4_main(void) {
  IfxCpu_enableInterrupts();
  IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());

  IfxCpu_emitEvent(&g_cpuSyncEvent);
  IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

  // CANflict Setup
  struct CANflict_SPI_t spi;
  spi.spi = &MODULE_QSPI0;
  spi.clkPin = &IfxQspi0_SCLK_P22_8_OUT;
  spi.mosiPin = &IfxQspi0_MTSR_P22_5_OUT;
  spi.misoPin = &IfxQspi0_MRSTC_P22_6_IN;
  spi.chipSelectPin = &IfxQspi2_SLSO0_P15_2_OUT;

  struct CANflict_SenderConf_t conf;
  conf.spi = &spi;
  conf.freqHz = CAN_BAUDRATE;
  conf.nSamples = N_SAMPLES;

  IfxPort_setPinModeInput(CAN0_TX->pin.port, CAN0_TX->pin.pinIndex,
                          IfxPort_InputMode_pullUp);
  CANflict_sender_init(&conf);
  CANflict_sender_setMsgToSend(&conf, MSG_BITS, MSG_BIT_LEN);

  // Send continuously with CANflict
  while (1) {
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 500));
    CANflict_sender_sendMsg(&conf);
  }
}
