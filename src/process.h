#ifndef PROCESS_H
#define PROCESS_H

#include <Arduino.h>
#include "config.h"

void setupWiFi();
String getCurrentTimeDate();
String getQuoteFromClaude(const String& timeDate);

#endif // PROCESS_H