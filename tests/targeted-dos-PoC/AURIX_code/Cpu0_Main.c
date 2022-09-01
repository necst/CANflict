#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"


/***************************************************
 *                    INTERRUPTS                   *
 ***************************************************
 * This core is used for interrupts, nothing is
 * executed in IDLE state.
 ***************************************************/

IFX_ALIGN(4) IfxCpu_syncEvent g_cpuSyncEvent = 0;


void core0_main(void) {
    IfxCpu_enableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    while (1) {
        // Leave CPU0 free for interrupts to be handled
    }
}


