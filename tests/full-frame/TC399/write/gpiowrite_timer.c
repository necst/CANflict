/*
 * \brief Example of how to use the GPIOSender_timer on the TC399XP TriBoard.
 *
 * To conduct this experiment, the TC399XP board has been wired as follows:
 *      - connected together pin3 (CANL) and pin4 (CANH) of the two CAN
 *        connectors (CAN0 and CAN1)
 *      - used P20.8 (CAN0TX) as naked pin
 *
 * The core were programmed as follows:
 * - cpu0 left idle for interrupt handling
 * - cpu1 programmed to read the CAN bus from the CAN1 interface
 *   using the MCMCAN example provided by Infineon
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
#include <senders/GPIOSender_timer.h>

#define CAN0_TX (&IfxCan_TXD00_P20_8_OUT) // Pin used for bitbanging
#define MSG_BIT_LEN 112
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
  struct CANflict_GPIO_t tx_gpio;
  tx_gpio.port = CAN0_TX->pin.port;
  tx_gpio.pin = CAN0_TX->pin.pinIndex;

  struct CANflict_TIM_t tim;
  tim.timer = &MODULE_GPT120;

  struct CANflict_SenderConf_t sndConf;
  sndConf.gpio = &tx_gpio;
  sndConf.tim = &tim;
  sndConf.blocking = true;
  sndConf.reloadVal = 18;

  CANflict_sender_init(&sndConf);

  // Send continuously with CANflict
  while (1) {
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 500));
    CANflict_sender_setMsgToSend(&sndConf, MSG_BITS, MSG_BIT_LEN);
    CANflict_sender_sendMsg(&sndConf);
  }
}
