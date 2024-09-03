#include <Arduino.h>
#include <WiFi.h>
#include <EEPROM.h>
#include "config.h"
#include "input.h"
#include "process.h"
#include "output.h"
#include "spiffs_utils.h"

#define FIRMWARE_VERSION "1.0.1"
#define PRINTER_ID_ADDRESS 0
#define PRINTER_ID_LENGTH 16

bool wifiConnected = false;
LedState currentLedState = WAITING_FOR_WIFI;
char printerId[PRINTER_ID_LENGTH + 1];  // +1 for null terminator

void generatePrinterId() {
    randomSeed(analogRead(0));
    for (int i = 0; i < PRINTER_ID_LENGTH; i++) {
        if (i % 2 == 0) {
            printerId[i] = random(48, 58);  // 0-9
        } else {
            printerId[i] = random(97, 123);  // a-z
        }
    }
    printerId[PRINTER_ID_LENGTH] = '\0';  // Null terminator
}

void loadOrGeneratePrinterId() {
    EEPROM.begin(PRINTER_ID_LENGTH);
    bool shouldGenerateNewId = false;

    for (int i = 0; i < PRINTER_ID_LENGTH; i++) {
        char c = EEPROM.read(PRINTER_ID_ADDRESS + i);
        if (c == 255) {  // Uninitialized EEPROM
            shouldGenerateNewId = true;
            break;
        }
        printerId[i] = c;
    }
    printerId[PRINTER_ID_LENGTH] = '\0';

    if (shouldGenerateNewId) {
        generatePrinterId();
        for (int i = 0; i < PRINTER_ID_LENGTH; i++) {
            EEPROM.write(PRINTER_ID_ADDRESS + i, printerId[i]);
        }
        EEPROM.commit();
    }

    Serial.print("Printer ID: ");
    Serial.println(printerId);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting setup...");
    Serial.print("Firmware version: ");
    Serial.println(FIRMWARE_VERSION);
    
    loadOrGeneratePrinterId();
    
    if (!initSPIFFS()) {
        Serial.println("Failed to initialize SPIFFS. Some features may not work.");
    }
    
    setupThermalPrinter();
    delay(3000);
    
    setupButton();
    setupLED();
    
    setupWiFi();
    
    currentLedState = WAITING_FOR_INPUT;
    
    Serial.println("Setup completed. Entering main loop.");
}

bool isWiFiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

void loop() {
    static unsigned long lastDebugTime = 0;
    static unsigned long loopCounter = 0;

    updateLED(currentLedState);
    loopCounter++;

    if (!wifiConnected) {
        wifiConnected = isWiFiConnected();
        if (wifiConnected) {
            Serial.println("WiFi connected");
        }
    }

    // Debug message for button state
    if (digitalRead(PIN_BUTTON) == LOW) {
        Serial.println("Button on pin 10 is pressed");
    }

    if (wifiConnected && isButtonPressed()) {
        Serial.println("Button press detected in main loop. Calling Claude...");
        
        currentLedState = WAITING_FOR_API;
        updateLED(currentLedState);
        
        String timeDate = getCurrentTimeDate();
        if (timeDate != "") {
            String quote = getQuoteFromClaude(timeDate);
            
            if (quote != "") {
                currentLedState = IDLE;
                updateLED(currentLedState);
                
                Serial.println("The Quote is:");
                Serial.println(quote);
                
                if (saveQuoteToSPIFFS(quote)) {
                    Serial.println("Quote saved to SPIFFS");
                } else {
                    Serial.println("Failed to save quote to SPIFFS");
                }
                
                printQuote(quote);
            } else {
                Serial.println("Failed to get quote from Claude. Trying to load last saved quote.");
                String lastQuote = loadQuoteFromSPIFFS();
                if (lastQuote != "") {
                    Serial.println("Printing last saved quote:");
                    Serial.println(lastQuote);
                    printQuote(lastQuote);
                } else {
                    Serial.println("No saved quote available.");
                }
            }
        } else {
            Serial.println("Failed to get current time and date.");
        }
        
        currentLedState = WAITING_FOR_INPUT;
    }

    // Add periodic debug output
    if (millis() - lastDebugTime > 5000) {  // Print debug info every 5 seconds
        Serial.println("-------- Debug Info --------");
        Serial.println("Firmware version: " + String(FIRMWARE_VERSION));
        Serial.println("Printer ID: " + String(printerId));
        Serial.println("Loop counter: " + String(loopCounter));
        Serial.println("WiFi connected: " + String(wifiConnected ? "Yes" : "No"));
        Serial.println("Current LED state: " + String(currentLedState));
        Serial.println(getSelectorStateString());
        Serial.println("Button state: " + String(digitalRead(PIN_BUTTON) == LOW ? "Pressed" : "Released"));
        Serial.println("-----------------------------");
        lastDebugTime = millis();
        loopCounter = 0;
    }
    
    delay(10);
}