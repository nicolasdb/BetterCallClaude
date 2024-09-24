#ifndef OUTPUT_H
#define OUTPUT_H

#include <Arduino.h>
#include "config.h"

void setupLED();
void updateLED(LedState state);
void setupThermalPrinter();
void printQuote(const String& quote);

#endif // OUTPUT_H