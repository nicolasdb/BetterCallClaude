#include "config.h"
#include "TestManager.h"
#include "feedback_display.h"

// Create FeedbackDisplay instance
FeedbackDisplay display(PIN_NEOPIXEL, 1);

// Global state variables
SystemState currentState = STATE_INIT;
unsigned long lastStateTime = 0;
bool buttonPressed = false;

// Color definitions
const uint32_t COLOR_BLUE = ((uint32_t)0 << 16) | ((uint32_t)0 << 8) | 255;    // Blue
const uint32_t COLOR_CYAN = ((uint32_t)0 << 16) | ((uint32_t)255 << 8) | 255;  // Cyan
const uint32_t COLOR_GREEN = ((uint32_t)0 << 16) | ((uint32_t)255 << 8) | 0;   // Green
const uint32_t COLOR_RED = ((uint32_t)255 << 16) | ((uint32_t)0 << 8) | 0;     // Red

void IRAM_ATTR handleButtonPress() {
    buttonPressed = true;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Initialize display
    display.begin();
    
    // Setup button interrupt
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), handleButtonPress, FALLING);
    
    // Initial state
    currentState = STATE_INIT;
    lastStateTime = millis();
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Update LED effect
    display.update();
    
    // State machine
    switch(currentState) {
        case STATE_INIT:
            if (currentMillis - lastStateTime >= 1000) {
                Serial.println("\n=== Quote Generator Starting ===");
                #if CURRENT_TEST_MODE == TEST_MODE_SPIFFS
                Serial.println("Mode: SPIFFS Test");
                #elif CURRENT_TEST_MODE == TEST_MODE_API
                Serial.println("Mode: API Test");
                #endif
                currentState = STATE_WAIT_BUTTON;
                display.setEffect(LED_BREATHE, COLOR_BLUE);  // Blue breathing
                lastStateTime = currentMillis;
            }
            break;
            
        case STATE_WAIT_BUTTON:
            if (buttonPressed) {
                buttonPressed = false;
                #if CURRENT_TEST_MODE == TEST_MODE_API
                currentState = STATE_WIFI_CONNECT;
                display.setEffect(LED_BREATHE, COLOR_CYAN);  // Cyan breathing
                #else
                currentState = STATE_TEST_RUNNING;
                display.setEffect(LED_BLINK, COLOR_GREEN, 3);  // Green blink
                #endif
                lastStateTime = currentMillis;
            }
            break;

        case STATE_WIFI_CONNECT:
            #if CURRENT_TEST_MODE == TEST_MODE_API
            if (TestManager::initWiFi()) {
                currentState = STATE_TEST_RUNNING;
                display.setEffect(LED_BLINK, COLOR_GREEN, 3);  // Green blink
            } else {
                currentState = STATE_ERROR;
                display.setEffect(LED_STATIC, COLOR_RED);  // Red static
            }
            #endif
            break;

        case STATE_TEST_RUNNING:
            display.setEffect(LED_BREATHE, COLOR_CYAN);  // Cyan breathing
            #if CURRENT_TEST_MODE == TEST_MODE_SPIFFS
            TestManager::runSPIFFSTest();
            #elif CURRENT_TEST_MODE == TEST_MODE_API
            TestManager::runAPITest();
            #endif
            currentState = STATE_TEST_COMPLETE;
            display.setEffect(LED_BREATHE, COLOR_GREEN);  // Green breathing
            break;
            
        case STATE_TEST_COMPLETE:
            if (buttonPressed) {
                buttonPressed = false;
                // Reset to wait for next test
                currentState = STATE_WAIT_BUTTON;
                display.setEffect(LED_BREATHE, COLOR_BLUE);  // Blue breathing
            }
            break;
            
        case STATE_ERROR:
            if (buttonPressed) {
                buttonPressed = false;
                // Return to wait state for retry
                currentState = STATE_WAIT_BUTTON;
                display.setEffect(LED_BREATHE, COLOR_BLUE);  // Blue breathing
            }
            break;
    }
    
    // Print status every 5 seconds
    static unsigned long lastPrint = 0;
    if (currentMillis - lastPrint >= 5000) {
        lastPrint = currentMillis;
        TestManager::printState(currentState);
    }
}
