#pragma once

#include <Ifx_Types.h>

// Priority of the timer interrupt
#define ISR_PRIORITY_GPT12_TIMER 6
// Interrupt provider
#define ISR_PROVIDER_GPT12_TIMER IfxSrc_Tos_cpu0

// Priority of SPI interrupt
#define ISR_PRIORITY_SPI_TX 50
#define ISR_PRIORITY_SPI_RX 51
#define ISR_PRIORITY_SPI_ER 52
// Interrupt provider
#define ISR_PROVIDER_SPI IfxSrc_Tos_cpu0

// Priority of UART interrupt
#define ISR_PRIORITY_UART_TX 18
#define ISR_PRIORITY_UART_RX 19
// Interrupt provider
#define ISR_PROVIDER_UART IfxSrc_Tos_cpu0