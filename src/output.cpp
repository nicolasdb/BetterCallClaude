#include "output.h"
#include "config.h"
#include <FastLED.h>
#include <Adafruit_Thermal.h>

CRGB leds[NUM_LEDS];
Adafruit_Thermal printer(&THERMAL_PRINTER_SERIAL);

void setupLED() {
    FastLED.addLeds<LED_TYPE, PIN_LED, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS);
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
                leds[0] = ledOn ? CRGB::Red : CRGB::Black;
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
    while (THERMAL_PRINTER_SERIAL.available() > 0) {
        THERMAL_PRINTER_SERIAL.read();
    }
}

void setupThermalPrinter() {
    THERMAL_PRINTER_SERIAL.begin(9600, SERIAL_8N1, THERMAL_PRINTER_RX, THERMAL_PRINTER_TX);
    delay(1000);  // Wait for the printer to initialize
    clearPrinterBuffer();
    printer.begin();
    printer.setDefault();  // Restore printer to defaults
    printer.wake();        // Wake up the printer
    printer.setFont('A');  // Set font to normal
    
    // Send reset command
    printer.write(27);
    printer.write(64);
    delay(50);  // Wait for the printer to process the reset command
    
    clearPrinterBuffer();
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

void printQuote(const String& quote) {
    clearPrinterBuffer();
    String formattedQuote = wordWrap(quote, THERMAL_PRINTER_MAX_CHAR_PER_LINE);
    printer.println(formattedQuote);
    printer.feed(2);
}