#include "output.h"
#include "config.h"
#include "input.h"
#include <FastLED.h>
#include <Adafruit_Thermal.h>

CRGB leds[NUM_LEDS];
Adafruit_Thermal printer(&THERMAL_PRINTER_SERIAL);

static bool isPrinterBusy = false;

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
            break;
        case WAITING_FOR_INPUT:
            if (millis() - lastToggle >= blinkInterval) {
                ledOn = !ledOn;
                leds[0] = ledOn ? CRGB::Green : CRGB::Black;
                lastToggle = millis();
            }
            break;
        case WAITING_FOR_API:
            leds[0] = CHSV(hue++, 255, 255);
            break;
        case IDLE:
            leds[0] = CRGB::Black;
            break;
    }
    FastLED.show();
}

void clearPrinterBuffer() {
    int bytesCleared = 0;
    while (THERMAL_PRINTER_SERIAL.available() > 0) {
        THERMAL_PRINTER_SERIAL.read();
        bytesCleared++;
    }
    Serial.printf("Printer buffer cleared. Bytes removed: %d\n", bytesCleared);
}

void setupThermalPrinter() {
    Serial.println("Setting up thermal printer...");
    
    Serial.printf("Initializing UART communication (RX: %d, TX: %d, Baud: %d)...\n", 
                  THERMAL_PRINTER_RX, THERMAL_PRINTER_TX, THERMAL_PRINTER_BAUD_RATE);
    
    // Explicitly initialize Serial1
    Serial1.begin(THERMAL_PRINTER_BAUD_RATE, SERIAL_8N1, THERMAL_PRINTER_RX, THERMAL_PRINTER_TX);
    
    if (!Serial1) {
        Serial.println("Failed to initialize Serial1 for thermal printer!");
        return;
    }
    
    Serial.println("Serial1 initialized for thermal printer");
    
    delay(3000);
    Serial.println("3-second delay complete");
    
    clearPrinterBuffer();
    
    Serial.println("Calling printer.begin()...");
    printer.begin();
    Serial.println("Printer begin() called");
    
    Serial.println("Setting printer defaults...");
    printer.setDefault();
    
    Serial.println("Waking up printer...");
    printer.wake();
    printer.setFont('A');
    
    Serial.println("Sending reset command to printer...");
    printer.write(27);
    printer.write(64);
    delay(50);
    
    clearPrinterBuffer();
    
    Serial.println("Thermal printer setup completed");
    
    // Test print
    // testPrinterCommunication();
}

void testPrinterCommunication() {
    Serial.println("Testing printer communication...");
    printer.println("Printer Test");
    printer.println("------------");
    printer.println("If you can read this,");
    printer.println("the printer is working.");
    printer.feed(2);
    Serial.println("Test print command sent. Check if the test message was printed.");
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
    if (isPrinterBusy) {
        Serial.println("Printer is busy. Cannot print quote.");
        return;
    }

    isPrinterBusy = true;
    Serial.println("Attempting to print quote...");
    clearPrinterBuffer();
    
    #if DEBUG_MODE
    String debugInfo = getSelectorStateString();
    printer.println(debugInfo);
    Serial.println("Debug info sent to printer: " + debugInfo);
    printer.println("--------------------");
    Serial.println("Separator sent to printer");
    #endif
    
    String formattedQuote = wordWrap(quote, THERMAL_PRINTER_MAX_CHAR_PER_LINE);
    printer.println(formattedQuote);
    Serial.println("Quote sent to printer: " + formattedQuote);
    
    printer.feed(2);
    Serial.println("Paper feed command sent");

    isPrinterBusy = false;
    Serial.println("Print operation completed");
}