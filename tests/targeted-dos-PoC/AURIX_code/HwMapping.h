/*
 * HwMapping.h
 *
 *  Created on: Mar 31, 2021
 *      Author: elvis
 */

#ifndef HWMAPPING_H_
#define HWMAPPING_H_

#include "IfxPort.h"

#define LED1        &MODULE_P33,4
#define LED2        &MODULE_P33,5
#define LED3        &MODULE_P33,6
#define LED4        &MODULE_P33,7
#define LED5        &MODULE_P20,11
#define LED6        &MODULE_P20,12
#define LED7        &MODULE_P20,13
#define LED8        &MODULE_P20,14
#define N_LED       8

//#define DEBUG_GPIO  &MODULE_P22,7
#define DEBUG_GPIO  &MODULE_P00,12
#define DEBUG_GPIO2  &MODULE_P22,9
#define DEBUG_GPIO3  &MODULE_P10,2

#define BUTTON      &MODULE_P33,11

void initPins(void);

#endif /* HWMAPPING_H_ */
