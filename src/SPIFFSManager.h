#ifndef SPIFFS_MANAGER_H
#define SPIFFS_MANAGER_H

#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "config.h"

class SPIFFSManager {
public:
    static bool begin();
    static bool saveQuote(const String& quote);
    static String loadLastQuote();
    static void listQuotes();

private:
    static String generateQuoteFilename();
    static void cleanupQuotes();
};

#endif // SPIFFS_MANAGER_H
