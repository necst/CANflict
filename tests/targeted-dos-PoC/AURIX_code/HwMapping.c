/*
 * HwMapping.c
 *
 *  Created on: Mar 31, 2021
 *      Author: elvis
 */
#include "HwMapping.h"

const IfxPort_Pin_Config g_leds[N_LED] = {
    {LED1, IfxPort_OutputIdx_general, IfxPort_PadDriver_cmosAutomotiveSpeed1},
    {LED2, IfxPort_OutputIdx_general, IfxPort_PadDriver_cmosAutomotiveSpeed1},
    {LED3, IfxPort_OutputIdx_general, IfxPort_PadDriver_cmosAutomotiveSpeed1},
    {LED4, IfxPort_OutputIdx_general, IfxPort_PadDriver_cmosAutomotiveSpeed1},
    {LED5,  IfxPort_OutputIdx_general, IfxPort_PadDriver_cmosAutomotiveSpeed1},
    {LED6,  IfxPort_OutputIdx_general, IfxPort_PadDriver_cmosAutomotiveSpeed1},
    {LED7,  IfxPort_OutputIdx_general, IfxPort_PadDriver_cmosAutomotiveSpeed1},
    {LED8,  IfxPort_OutputIdx_general, IfxPort_PadDriver_cmosAutomotiveSpeed1}
};

void initPins() {
    // LEDS
    for(size_t i = 0; i < N_LED; ++i) {
        IfxPort_setPinModeOutput(g_leds[i].port, g_leds[i].pinIndex, IfxPort_OutputMode_pushPull, g_leds[i].mode);
        IfxPort_setPinPadDriver(g_leds[i].port, g_leds[i].pinIndex, g_leds[i].padDriver);
        IfxPort_setPinHigh(g_leds[i].port, g_leds[i].pinIndex);
    }

    // DEBUG pin
    IfxPort_setPinModeOutput(DEBUG_GPIO, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinPadDriver(DEBUG_GPIO, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxPort_setPinHigh(DEBUG_GPIO);

    IfxPort_setPinModeOutput(DEBUG_GPIO2, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinPadDriver(DEBUG_GPIO2, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxPort_setPinHigh(DEBUG_GPIO2);

    IfxPort_setPinModeOutput(DEBUG_GPIO3, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinPadDriver(DEBUG_GPIO3, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxPort_setPinHigh(DEBUG_GPIO3);

    // Button pin
    IfxPort_setPinModeInput(BUTTON, IfxPort_InputMode_pullUp);
}
