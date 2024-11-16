#include "spiffs_utils.h"

#define QUOTE_FILENAME "/last_quote.txt"

bool initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An error occurred while mounting SPIFFS");
        return false;
    }
    return true;
}

bool saveQuoteToSPIFFS(const String& quote) {
    File file = SPIFFS.open(QUOTE_FILENAME, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    if (file.print(quote)) {
        Serial.println("Quote saved to SPIFFS");
        file.close();
        return true;
    } else {
        Serial.println("Failed to save quote to SPIFFS");
        file.close();
        return false;
    }
}

String loadQuoteFromSPIFFS() {
    File file = SPIFFS.open(QUOTE_FILENAME, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return "";
    }
    String quote = file.readString();
    file.close();
    return quote;
}