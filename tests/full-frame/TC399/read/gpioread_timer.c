/*
 * \brief Example of how to use the GPIOReceiver_poll on the TC399XP TriBoard.
 *
 * To conduct this experiment, the TC399XP board has been wired as follows:
 *      - connected together pin3 (CANL) and pin4 (CANH) of the two CAN
 *        connectors (CAN0 and CAN1)
 *      - used P20.7 (CAN0RX) as naked pin
 *      - pin P33.4 attached to the on-board LED to signal an error
 *
 * The core were programmed as follows:
 * - cpu0 left idle for interrupt handling
 * - cpu1 programmed to continuously write a message
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
#include <receivers/GPIOReceiver_timer.h>

#define CAN0_RX (&IfxCan_RXD00B_P20_7_IN) // Pin used for bitbanging
#define CAN0_TX (&IfxCan_TXD00_P20_8_OUT) // Pin used for bitbanging
#define LED &MODULE_P33, 4

#define MSG_BIT_LEN 103
static const bool EXPECTED_MSG[MSG_BIT_LEN] = {
    0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
    1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1,
    0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0,
    1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1}; //, /*ACK:*/ 1};
                                                              //// , 1, 1, 1,
// 1, 1, 1, 1, 1};

extern IfxCpu_syncEvent g_cpuSyncEvent;

static inline void initLed(void) {
  IfxPort_setPinModeOutput(LED, IfxPort_OutputMode_pushPull,
                           IfxPort_OutputIdx_general);
  IfxPort_setPinPadDriver(LED, IfxPort_PadDriver_cmosAutomotiveSpeed1);
  IfxPort_setPinHigh(LED);
}

static inline void sendAck(void) {
  for (int i = 0; i < 95; i++)
    ;
  IfxPort_setPinLow(tx_gpio.port, tx_gpio.pin);
  for (int i = 0; i < 85; i++)
    ;
  IfxPort_setPinHigh(tx_gpio.port, tx_gpio.pin);
}

void core4_main(void) {
  IfxCpu_enableInterrupts();
  IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());

  IfxCpu_emitEvent(&g_cpuSyncEvent);
  IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

  initLed();

  // CANTX pin for sending the ACK
  struct CANflict_GPIO_t tx_gpio;
  tx_gpio.port = CAN0_TX->pin.port;
  tx_gpio.pin = CAN0_TX->pin.pinIndex;
  CANflict_GPIO_init(&tx_gpio, false);

  // CANflict Setup
  struct CANflict_GPIO_t tx_gpio;
  tx_gpio.port = CAN0_TX->pin.port;
  tx_gpio.pin = CAN0_TX->pin.pinIndex;

  struct CANflict_TIM_t tim;
  tim.timer = &MODULE_GPT120;

  struct CANflict_ReceiverConf_t sndConf;
  sndConf.gpio = &tx_gpio;
  sndConf.tim = &tim;
  sndConf.blocking = true;
  sndConf.reloadVal = 18;

  bool rcvBuf[MSG_BIT_LEN];

  CANflict_receiver_init(&rcvConf);

  while (1) {
    // Wait for falling edges
    while (IfxPort_getPinState(rx_gpio.port, rx_gpio.pin) == true)
      ;

    // Read bits until ACK
    CANflict_receiver_rcvBits(&rcvConf, rcvBuf, MSG_BIT_LEN);
    sendAck();

    // Check received bits
    int i = 0;
    for (i = 0; i < MSG_BIT_LEN; i++) {
      if ((rcvBuf[i] & 0x1) != (EXPECTED_MSG[i] & 0x1)) {
        IfxPort_togglePin(LED);
        break;
      }
    }
  }
}
