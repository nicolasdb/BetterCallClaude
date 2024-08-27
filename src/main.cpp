#include <Arduino.h>
#include "config.h"
#include "input.h"
#include "process.h"
#include "output.h"

bool wifiConnected = false;
LedState currentLedState = WAITING_FOR_WIFI;

void setup() {
    Serial.begin(115200);
    
    setupButton();
    setupLED();
    setupThermalPrinter();
    
    setupWiFi();
    currentLedState = WAITING_FOR_INPUT;
}

void loop() {
    updateLED(currentLedState);

    if (wifiConnected && isButtonPressed()) {
        Serial.println("Button pressed. Calling Claude...");
        
        currentLedState = WAITING_FOR_API;
        updateLED(currentLedState);
        
        String timeDate = getCurrentTimeDate();
        if (timeDate != "") {
            String quote = getQuoteFromClaude(timeDate);
            
            currentLedState = IDLE;
            updateLED(currentLedState);
            
            Serial.println("The Quote is:");
            Serial.println(quote);
            
            printQuote(quote);
        } else {
            Serial.println("Failed to get current time and date.");
        }
        
        currentLedState = WAITING_FOR_INPUT;
    }
    
    delay(10);
}