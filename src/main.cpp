#include "config.h"
#include "feedback_display.h"
#include "printer.h"
#include "input_manager.h"
#include "json_parser.h"
#include "SPIFFSManager.h"
#include "wifi_manager.h"
#include "APIClient.h"

// Create instances
FeedbackDisplay display(PIN_NEOPIXEL, 1);
Printer printer;
InputManager inputManager;
APIClient apiClient;

// Global state variables
enum SystemState {
    STATE_INIT,
    STATE_IDLE,
    STATE_PROCESSING,
    STATE_ERROR
};

SystemState currentState = STATE_INIT;
unsigned long lastStateTime = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== Quote Generator Starting ===");

    // Initialize display
    display.begin();
    Serial.println("Display initialized");
    
    // Initialize printer
    if (!printer.begin()) {
        Serial.println("Printer initialization failed");
        display.setEffect(LED_STATIC, COLOR_RED);
        return;
    }
    Serial.println("Printer initialized");
    
    // Initialize input manager
    inputManager.begin();
    Serial.println("Input manager initialized");
    
    // Initialize SPIFFS
    if (!SPIFFSManager::begin()) {
        Serial.println("SPIFFS initialization failed");
        display.setEffect(LED_STATIC, COLOR_RED);
        return;
    }
    Serial.println("SPIFFS initialized");
    
    // Initialize WiFi manager
    wifi_manager_init();
    Serial.println("WiFi manager initialized");
    
    // Initialize API client
    if (!APIClient::begin()) {
        Serial.println("API client initialization failed");
        display.setEffect(LED_STATIC, COLOR_RED);
        return;
    }
    Serial.println("API client initialized");
    
    // Initial state
    currentState = STATE_IDLE;
    display.setEffect(LED_BREATHE, COLOR_BLUE);
}

void handleCase0() {
    // Read and print pitch.txt
    String pitch = SPIFFSManager::readFile("/pitch.txt");
    if (pitch.length() > 0) {
        printer.printQuote(pitch);
        Serial.println("Printed pitch.txt content");
        display.setEffect(LED_BREATHE, COLOR_GREEN);
    } else {
        Serial.println("Failed to read pitch.txt");
        display.setEffect(LED_STATIC, COLOR_RED);
        currentState = STATE_ERROR;
    }
}

void handleCase1() {
    // Use prompt selection 1 for API request
    wifi_manager_connect();  // Connect using stored credentials
    
    String response = APIClient::generateAndStoreQuote(1);  // Use prompt 1
    if (response.length() > 0) {
        String quote = JsonParser::extractQuote(response);
        if (quote.length() > 0) {
            printer.printQuote(quote);
            Serial.println("Printed quote from prompt 1");
            display.setEffect(LED_BREATHE, COLOR_GREEN);
        } else {
            Serial.println("Failed to extract quote from response");
            display.setEffect(LED_STATIC, COLOR_RED);
            currentState = STATE_ERROR;
        }
    } else {
        Serial.println("API request failed");
        display.setEffect(LED_STATIC, COLOR_RED);
        currentState = STATE_ERROR;
    }
}

void handleCase2() {
    // Use prompt selection 2 for API request
    wifi_manager_connect();  // Connect using stored credentials
    
    String response = APIClient::generateAndStoreQuote(2);  // Use prompt 2
    if (response.length() > 0) {
        String quote = JsonParser::extractQuote(response);
        if (quote.length() > 0) {
            printer.printQuote(quote);
            Serial.println("Printed quote from prompt 2");
            display.setEffect(LED_BREATHE, COLOR_GREEN);
        } else {
            Serial.println("Failed to extract quote from response");
            display.setEffect(LED_STATIC, COLOR_RED);
            currentState = STATE_ERROR;
        }
    } else {
        Serial.println("API request failed");
        display.setEffect(LED_STATIC, COLOR_RED);
        currentState = STATE_ERROR;
    }
}

void loop() {
    // Update LED effect
    display.update();
    
    // Handle system states
    switch(currentState) {
        case STATE_INIT:
            // Should not reach here after setup
            currentState = STATE_IDLE;
            display.setEffect(LED_BREATHE, COLOR_BLUE);
            break;
            
        case STATE_IDLE:
            if (inputManager.isExternalButtonPressed()) {
                display.setEffect(LED_BLINK, COLOR_CYAN, 3);
                currentState = STATE_PROCESSING;
                
                // Get selector position and handle the corresponding case
                InputManager::SelectorPosition position = inputManager.getSelectorPosition();
                switch(position) {
                    case InputManager::POSITION_0:
                        handleCase0();
                        break;
                    case InputManager::POSITION_1:
                        handleCase1();
                        break;
                    case InputManager::POSITION_2:
                        handleCase2();
                        break;
                    default:
                        Serial.println("Invalid selector position");
                        display.setEffect(LED_STATIC, COLOR_RED);
                        currentState = STATE_ERROR;
                        break;
                }
                
                // Return to IDLE if not in ERROR state
                if (currentState != STATE_ERROR) {
                    currentState = STATE_IDLE;
                }
            }
            break;
            
        case STATE_PROCESSING:
            // Processing is handled in the case functions
            break;
            
        case STATE_ERROR:
            // Reset to IDLE when builtin button is pressed
            if (inputManager.isBuiltinButtonPressed()) {
                currentState = STATE_IDLE;
                display.setEffect(LED_BREATHE, COLOR_BLUE);
            }
            break;
    }
}
