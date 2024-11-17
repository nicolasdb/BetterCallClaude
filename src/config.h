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

// SPIFFS Quote Storage Settings
#define QUOTES_MAX_STORED 5           // Maximum number of quotes to store
#define QUOTES_STORAGE_DIR "/quotes"  // Directory for storing quote JSON files
#define QUOTE_FILENAME_PREFIX "/quote_"  // Prefix for quote JSON files
#define QUOTE_FILE_EXT ".json"         // File extension for quote files

// Debug and Test Modes
#define DEBUG_MODE 1  // Set to 1 to enable debug output, 0 to disable

// Test Mode Selection (only enable one at a time)
#define TEST_MODE_NONE 0      // Normal operation
#define TEST_MODE_SPIFFS 1    // SPIFFS test mode
#define TEST_MODE_API 2       // API test mode

// Select which test to run (uncomment only one)
// #define CURRENT_TEST_MODE TEST_MODE_NONE
// #define CURRENT_TEST_MODE TEST_MODE_SPIFFS
#define CURRENT_TEST_MODE TEST_MODE_API

#endif  // CONFIG_H
