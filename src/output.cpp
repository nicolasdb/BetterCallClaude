#include "output.h"
#include "config.h"
#include "input.h"
#include <FastLED.h>
#include <Adafruit_Thermal.h>

CRGB leds[NUM_LEDS];
Adafruit_Thermal printer(&THERMAL_PRINTER_SERIAL);

void setupLED() {
    FastLED.addLeds<LED_TYPE, PIN_LED, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS);
    Serial.println("LED setup completed");
}

void updateLED(LedState state) {
    static uint8_t hue = 0;
    static bool ledOn = false;
    static unsigned long lastToggle = 0;
    const unsigned long blinkInterval = 500;

    switch (state) {
        case WAITING_FOR_WIFI:
            leds[0] = CHSV(160, 255, abs(sin(millis() / 500.0) * 255));
            Serial.println("LED: Waiting for WiFi");
            break;
        case WAITING_FOR_INPUT:
            if (millis() - lastToggle >= blinkInterval) {
                ledOn = !ledOn;
                leds[0] = ledOn ? CRGB::Red : CRGB::Black;
                lastToggle = millis();
                Serial.println("LED: Waiting for input (Blinking Red)");
            }
            break;
        case WAITING_FOR_API:
            leds[0] = CHSV(hue++, 255, 255);
            Serial.println("LED: Waiting for API (Rainbow)");
            break;
        case IDLE:
            leds[0] = CRGB::Black;
            Serial.println("LED: Idle (Off)");
            break;
    }
    FastLED.show();
}

void clearPrinterBuffer() {
    while (THERMAL_PRINTER_SERIAL.available() > 0) {
        THERMAL_PRINTER_SERIAL.read();
    }
    Serial.println("Printer buffer cleared");
}

void setupThermalPrinter() {
    Serial.println("Setting up thermal printer...");
    
    // Initialize UART communication
    THERMAL_PRINTER_SERIAL.begin(THERMAL_PRINTER_BAUD_RATE, SERIAL_8N1, THERMAL_PRINTER_RX, THERMAL_PRINTER_TX);
    Serial.println("Thermal printer serial initialized");
    
    // Add a delay before sending data
    delay(3000);
    Serial.println("Delay complete");
    
    // Flush the UART buffer
    clearPrinterBuffer();
    
    Serial.println("Calling printer.begin()...");
    printer.begin();
    Serial.println("Printer begin() called");
    
    Serial.println("Setting printer defaults...");
    printer.setDefault();
    
    Serial.println("Waking up printer...");
    printer.wake();
    
    Serial.println("Setting printer font...");
    printer.setFont('A');
    
    Serial.println("Sending reset command to printer...");
    printer.write(27);
    printer.write(64);
    delay(50);  // Wait for the printer to process the reset command
    
    Serial.println("Final clear of printer buffer...");
    clearPrinterBuffer();
    
    Serial.println("Thermal printer setup completed");
}

String wordWrap(const String& text, int lineWidth) {
    String result = "";
    String currentLine = "";
    int lastSpace = -1;

    for (unsigned int i = 0; i < text.length(); i++) {
        currentLine += text[i];

        if (text[i] == ' ') {
            lastSpace = currentLine.length() - 1;
        }

        if (currentLine.length() == lineWidth) {
            if (lastSpace != -1) {
                result += currentLine.substring(0, lastSpace) + "\n";
                currentLine = currentLine.substring(lastSpace + 1);
                lastSpace = -1;
            } else {
                result += currentLine + "\n";
                currentLine = "";
            }
        }
    }

    if (currentLine.length() > 0) {
        result += currentLine;
    }

    return result;
}

String getSelectorStateString() {
    int selectorState = getSystemPromptSelector();
    return "Selector State: " + String(selectorState);
}

void printQuote(const String& quote) {
    Serial.println("Printing quote...");
    clearPrinterBuffer();
    
    #if DEBUG_MODE
    String debugInfo = getSelectorStateString();
    printer.println(debugInfo);
    printer.println("--------------------");
    Serial.println("Debug info printed: " + debugInfo);
    #endif
    
    String formattedQuote = wordWrap(quote, THERMAL_PRINTER_MAX_CHAR_PER_LINE);
    printer.println(formattedQuote);
    printer.feed(2);
    Serial.println("Quote printed successfully");
}