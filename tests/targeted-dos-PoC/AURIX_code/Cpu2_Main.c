#include "Bsp.h"
#include "HwMapping.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"
#include "MCMCAN.h"
#include "MsgTable.h"

/***************************************************
 *                      TARGET                     *
 ***************************************************
 * This core is used to command the sending of target
 * messages from CAN0.
 ***************************************************/

extern IfxCpu_syncEvent g_cpuSyncEvent;

void core2_main(void) {
    IfxCpu_enableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());

    // Init pins and shell
    initPins();
    // Init CANBus peripheral
    initSingleMcmcan(&g_mcmcan0, /*canId*/0, /*loopback=*/FALSE);

    // Initial button state
    boolean buttonState = IfxPort_getPinState(BUTTON);

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    for (unsigned loopIdx = 0;; loopIdx++) {
        // Wait for the button to be pressed
        while (buttonState == IfxPort_getPinState(BUTTON)) {}

        // CAN0 -> CAN1
        for (int i = 0; i < N_TARGET_MSG; i++) {
            Msg msg = target[i];
            waitTime(IfxStm_getTicksFromMicroseconds(BSP_DEFAULT_TIMER, msg.delay_us));
            transmitFullCanMessage(/*canId=*/0, msg.ID, msg.payload, /*size=*/8);
        }

        // Debounce
        waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 200));

    }
}
