#ifndef PROCESS_H
#define PROCESS_H

#include <Arduino.h>
#include "config.h"

void setupWiFi();
String getQuoteFromClaude(int promptSelector);

#endif // PROCESS_H