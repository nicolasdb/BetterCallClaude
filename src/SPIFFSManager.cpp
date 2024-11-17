#include "SPIFFSManager.h"

bool SPIFFSManager::begin() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An error occurred while mounting SPIFFS");
        return false;
    }
    return true;
}

String SPIFFSManager::generateQuoteFilename() {
    static int fileCounter = 0;
    fileCounter = (fileCounter + 1) % QUOTES_MAX_STORED;
    
    char filename[32];
    snprintf(filename, sizeof(filename), "/quote_%d.json", fileCounter);
    return String(filename);
}

bool SPIFFSManager::saveQuote(const String& quote) {
    String filename = generateQuoteFilename();
    File file = SPIFFS.open(filename, FILE_WRITE);
    
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    
    StaticJsonDocument<1024> doc;
    doc["quote"] = quote;
    doc["timestamp"] = millis();
    
    bool success = serializeJson(doc, file) > 0;
    file.close();
    
    if (success && DEBUG_MODE) {
        Serial.println("Quote saved to: " + filename);
    }
    
    return success;
}

String SPIFFSManager::loadLastQuote() {
    File root = SPIFFS.open("/");
    if (!root) {
        Serial.println("Failed to open root directory");
        return "";
    }

    String lastQuote;
    unsigned long latestTimestamp = 0;
    
    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory() && String(file.name()).endsWith(".json")) {
            StaticJsonDocument<1024> doc;
            String content = file.readString();
            DeserializationError error = deserializeJson(doc, content);
            
            if (!error) {
                unsigned long timestamp = doc["timestamp"] | 0;
                if (timestamp > latestTimestamp) {
                    latestTimestamp = timestamp;
                    lastQuote = doc["quote"].as<String>();
                }
            }
        }
        file = root.openNextFile();
    }
    root.close();
    
    if (lastQuote.isEmpty()) {
        Serial.println("No quotes found");
    }
    
    return lastQuote;
}

void SPIFFSManager::listQuotes() {
    File root = SPIFFS.open("/");
    if (!root) {
        Serial.println("Failed to open root directory");
        return;
    }
    
    Serial.println("Stored Quotes:");
    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory() && String(file.name()).endsWith(".json")) {
            Serial.println(file.name());
        }
        file = root.openNextFile();
    }
    root.close();
}

void SPIFFSManager::cleanupQuotes() {
    // This is handled automatically by generateQuoteFilename()
    // which cycles through a fixed number of filenames
}
