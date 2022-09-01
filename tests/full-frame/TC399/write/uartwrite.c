/*
 * \brief Example of how to use the USARTSender on the TC399XP TriBoard.
 *
 * To conduct this experiment, the TC399XP board has been wired as follows:
 *      - connected together pin3 (CANL) and pin4 (CANH) of the two CAN
 *        connectors (CAN0 and CAN1)
 *      - connected P20.8 (CAN0TX) to P10.4 (ASCLIN TX)
 *
 * The cores were programmed as follows:
 * - cpu0 left idle for interrupt handling
 * - cpu1 programmed to read the CAN bus using the MCMCAN example provided
 *   by Infineon
 * - cpu4 programmed with the following code
 */

// Target headers
#include "Bsp.h"
#include "IfxAsclin_Asc.h"
#include "IfxCan_PinMap.h"
#include "IfxCpu.h"
#include "IfxCpu_Irq.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"


// CANflict headers
#include <platforms/AURIX/platform.h>
#include <senders/UARTSender.h>

static const bool MSG_BITS[] = {0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
                                1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1,
                                0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0,
                                1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1};
static const uint32_t MSG_BIT_LEN = 58;
static const uint8_t MSG_BYTE_LEN = (58 / 8) + 1;

#define BAUDRATE 1000000 // = 1 MBit/s

extern IfxCpu_syncEvent g_cpuSyncEvent;

void core4_main(void) {
  IfxCpu_enableInterrupts();
  IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());

  IfxCpu_emitEvent(&g_cpuSyncEvent);
  IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

  // CANflict Setup
  struct CANflict_UART_t uart;
  uart.uart = &MODULE_ASCLIN2;
  uart.rxPin = &IfxAsclin2_RXA_P14_3_IN;
  uart.txPin = &IfxAsclin2_TX_P10_5_OUT;

  struct CANflict_SenderConf_t conf;
  conf.uart = &uart;
  conf.freqHz = BAUDRATE;
  conf.wordLen = 8;
  conf.stopBits = 1;

  uint8_t msgBytes[MSG_BYTE_LEN];
  uint8_t rxBuffer[MSG_BYTE_LEN];
  uint8_t txBuffer[MSG_BYTE_LEN];

  conf.msgBytes = msgBytes;
  conf.byteLen = MSG_BYTE_LEN;

  uart.txBufferSize = MSG_BYTE_LEN;
  uart.txBuffer = txBuffer;
  uart.rxBufferSize = MSG_BYTE_LEN;
  uart.rxBuffer = rxBuffer;

  CANflict_sender_init(&conf);
  CANflict_sender_setMsgToSend(&conf, MSG_BITS, MSG_BIT_LEN);

  // Send continuously with CANflict
  while (1) {
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 500));
    CANflict_sender_sendMsg(&conf);
  }
}
