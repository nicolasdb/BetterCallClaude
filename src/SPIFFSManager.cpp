#include "SPIFFSManager.h"
#include <vector>
#include <algorithm>

bool SPIFFSManager::begin() {
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return false;
    }
    return true;
}

String SPIFFSManager::readFile(const String& path) {
    if (!SPIFFS.exists(path)) {
        Serial.printf("File %s does not exist\n", path.c_str());
        return "";
    }

    File file = SPIFFS.open(path, "r");
    if (!file) {
        Serial.printf("Failed to open file %s for reading\n", path.c_str());
        return "";
    }

    String content = file.readString();
    file.close();
    return content;
}

bool SPIFFSManager::saveQuote(const String& quote) {
    String filename = generateQuoteFilename();
    File file = SPIFFS.open(filename, "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    
    StaticJsonDocument<1024> doc;
    doc["quote"] = quote;
    doc["timestamp"] = millis();
    
    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write JSON to file");
        file.close();
        return false;
    }
    
    file.close();
    cleanupQuotes();
    return true;
}

String SPIFFSManager::loadLastQuote() {
    File root = SPIFFS.open("/");
    if (!root) {
        Serial.println("Failed to open directory");
        return "";
    }
    
    String lastFilename;
    File file = root.openNextFile();
    while (file) {
        String filename = String(file.name());
        if (filename.startsWith(QUOTE_FILENAME_PREFIX) && 
            filename.endsWith(QUOTE_FILE_EXT)) {
            if (lastFilename.length() == 0 || filename > lastFilename) {
                lastFilename = filename;
            }
        }
        file = root.openNextFile();
    }
    
    if (lastFilename.length() == 0) {
        Serial.println("No quote files found");
        return "";
    }
    
    File quoteFile = SPIFFS.open(lastFilename, "r");
    if (!quoteFile) {
        Serial.println("Failed to open quote file");
        return "";
    }
    
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, quoteFile);
    quoteFile.close();
    
    if (error) {
        Serial.println("Failed to parse JSON");
        return "";
    }
    
    return doc["quote"].as<String>();
}

void SPIFFSManager::listQuotes() {
    File root = SPIFFS.open("/");
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    
    Serial.println("Quotes in storage:");
    File file = root.openNextFile();
    while (file) {
        String filename = String(file.name());
        if (filename.startsWith(QUOTE_FILENAME_PREFIX) && 
            filename.endsWith(QUOTE_FILE_EXT)) {
            Serial.printf("- %s (%d bytes)\n", filename.c_str(), file.size());
        }
        file = root.openNextFile();
    }
}

String SPIFFSManager::generateQuoteFilename() {
    unsigned long timestamp = millis();
    return String(QUOTE_FILENAME_PREFIX) + String(timestamp) + String(QUOTE_FILE_EXT);
}

void SPIFFSManager::cleanupQuotes() {
    File root = SPIFFS.open("/");
    if (!root) {
        Serial.println("Failed to open directory for cleanup");
        return;
    }
    
    // Store filenames in a simple array since we have a small max
    String quoteFiles[QUOTES_MAX_STORED + 1];  // +1 for potential overflow
    int fileCount = 0;
    
    File file = root.openNextFile();
    while (file) {
        String filename = String(file.name());
        if (filename.startsWith(QUOTE_FILENAME_PREFIX) && 
            filename.endsWith(QUOTE_FILE_EXT)) {
            quoteFiles[fileCount++] = filename;
            if (fileCount >= QUOTES_MAX_STORED + 1) break;
        }
        file = root.openNextFile();
    }
    
    // If we have more files than allowed, remove the oldest one
    if (fileCount > QUOTES_MAX_STORED) {
        // Find oldest file (smallest timestamp)
        String oldestFile = quoteFiles[0];
        for (int i = 1; i < fileCount; i++) {
            if (quoteFiles[i] < oldestFile) {
                oldestFile = quoteFiles[i];
            }
        }
        
        // Remove oldest file
        if (SPIFFS.remove(oldestFile)) {
            Serial.printf("Removed old quote file: %s\n", oldestFile.c_str());
        } else {
            Serial.printf("Failed to remove old quote file: %s\n", oldestFile.c_str());
        }
    }
}
