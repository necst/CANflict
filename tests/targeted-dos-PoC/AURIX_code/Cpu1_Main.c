#include "Bsp.h"
#include "HwMapping.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"
#include "MCMCAN.h"
#include "MsgTable.h"

/***************************************************
 *                      NOISE                      *
 ***************************************************
 * This core is used to command the sending of noise
 * messages from CAN1.
 ***************************************************/

extern IfxCpu_syncEvent g_cpuSyncEvent;

void core1_main(void) {
    IfxCpu_enableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());

    // Init pins and shell
    initPins();
    // Init CANBus peripheral
    initSingleMcmcan(&g_mcmcan1, /*canId*/1, /*loopback=*/FALSE);

    // Initial button state
    boolean buttonState = IfxPort_getPinState(BUTTON);

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    for (unsigned loopIdx = 0;; loopIdx++) {
        // Wait for the button to be pressed
        while (buttonState == IfxPort_getPinState(BUTTON)) {}

        // CAN1 -> CAN0
        for (int i = 0; i < N_NOISE_MSG; i++) {
            Msg msg = noise[i];
            waitTime(IfxStm_getTicksFromMicroseconds(BSP_DEFAULT_TIMER, msg.delay_us));
            transmitFullCanMessage(/*canId=*/1, msg.ID, msg.payload, /*size=*/8);
        }

        // Debounce
        waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 200));

    }
}
