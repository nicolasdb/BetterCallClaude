#ifndef OUTPUT_H
#define OUTPUT_H

#include <Arduino.h>
#include "config.h"

enum LedState {
    WAITING_FOR_WIFI,
    WAITING_FOR_INPUT,
    WAITING_FOR_API,
    IDLE
};

void setupLED();
void updateLED(LedState state);
void setupThermalPrinter();
void printQuote(const String& quote);

#endif // OUTPUT_H