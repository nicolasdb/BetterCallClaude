#ifndef SPIFFS_UTILS_H
#define SPIFFS_UTILS_H

#include <Arduino.h>
#include <SPIFFS.h>

bool initSPIFFS();
bool saveQuoteToSPIFFS(const String& quote);
String loadQuoteFromSPIFFS();

#endif // SPIFFS_UTILS_H