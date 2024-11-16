#ifndef PROCESS_H
#define PROCESS_H

#include <Arduino.h>
#include "config.h"

void setupWiFi();
String getQuoteFromClaude(int promptSelector, float temperature, int maxTokens, int randomSeed);

#endif // PROCESS_H