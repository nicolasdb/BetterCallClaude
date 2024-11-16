#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions for different boards
#if defined(ESP32C3_DEV)
    #define PIN_BUTTON 9      // According to platformio.ini comment
    #define PIN_NEOPIXEL 2    // According to platformio.ini comment
    #define LED_TYPE SK6812
#elif defined(ARDUINO_M5STACK_STAMPS3)
    #define PIN_BUTTON 10
    #define PIN_NEOPIXEL  21
    #define LED_TYPE WS2812
#else
    #error "Unsupported board selected"
#endif

// Printer settings
#define PRINTER_BAUDRATE 9600
#define PRINTER_PORT "/dev/ttyUSB0"

#endif  // CONFIG_H
