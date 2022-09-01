#pragma once

///\file ADC.h
///\brief This file defines the interface of an abstract ADC peripheral.

#include <stdint.h>

bool CANflict_ADC_init();
bool CANflict_ADC_deinit();
// bool CANflict_ADC_setResolution();
// bool CANflict_ADC_setBitTime();
bool CANflict_ADC_setNumConversions(uint32_t nConversions);
bool CANflict_ADC_conversionCallback(
    uint32_t convertedValue); // TODO: setConversionCallback()
bool CANflict_ADC_startConversion();
bool CANflict_ADC_stopConversion();
