#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "credentials.h"
#include "config.h"

// Pin definitions for different boards
#if defined(CONFIG_IDF_TARGET_ESP32C3)
    #define PIN_BUTTON 9
    #define PIN_LED 2
    #define LED_TYPE SK6812
#elif defined(ARDUINO_M5STACK_STAMPS3)
    #define PIN_BUTTON 0
    #define PIN_LED 21
    #define LED_TYPE WS2812
#else
    #error "Unsupported board selected"
#endif

// Claude API configuration
const char* apiEndpoint = "https://api.anthropic.com/v1/messages";

// LED setup
CRGB leds[NUM_LEDS];

// LED states
enum LedState {
    WAITING_FOR_WIFI,
    WAITING_FOR_INPUT,
    WAITING_FOR_API,
    IDLE
};

// Global variables
LedState currentLedState = WAITING_FOR_WIFI;
TaskHandle_t ledTaskHandle = NULL;
TaskHandle_t wifiTaskHandle = NULL;
bool serialAvailable = false;
bool wifiConnected = false;

// Function prototypes
void setupWiFi(void * parameter);
String getCurrentTimeDate();
String getQuoteFromClaude(const String& timeDate);
void ledTask(void * parameter);
void debugPrint(const String& message, bool always = false);

void setup() {
    Serial.begin(115200);
    
    // Wait for serial connection or timeout after 3 seconds
    unsigned long startTime = millis();
    while (!Serial && millis() - startTime < 3000) {
        delay(100);
    }
    serialAvailable = Serial;
    
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    FastLED.addLeds<LED_TYPE, PIN_LED, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS);
    
    // Create the LED task
    xTaskCreatePinnedToCore(
        ledTask,
        "LEDTask",
        2048,
        NULL,
        1,
        &ledTaskHandle,
        0
    );
    
    // Create the WiFi connection task
    xTaskCreatePinnedToCore(
        setupWiFi,
        "WiFiTask",
        4096,
        NULL,
        1,
        &wifiTaskHandle,
        1
    );
    
    debugPrint("Setup complete. Connecting to WiFi...", true);
}

void loop() {
    static unsigned long lastDebounceTime = 0;
    static const unsigned long debounceDelay = 50;

    if (wifiConnected && digitalRead(PIN_BUTTON) == LOW) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
            debugPrint("Button pressed. I should better call Claude...", true);
            
            currentLedState = WAITING_FOR_API;
            
            String timeDate = getCurrentTimeDate();
            if (timeDate != "") {
                String quote = getQuoteFromClaude(timeDate);
                
                currentLedState = IDLE;
                
                debugPrint("Socratic Quote:", true);
                debugPrint(quote, true);
            } else {
                debugPrint("Failed to get current time and date.", true);
            }
            
            currentLedState = WAITING_FOR_INPUT;
        }
        lastDebounceTime = millis();
    }
    delay(10);
}

// Sets up WiFi connection with retry mechanism
void setupWiFi(void * parameter) {
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        debugPrint(".");
        attempts++;
    }
    if (WiFi.status() != WL_CONNECTED) {
        debugPrint("\nFailed to connect to WiFi. Restarting...", true);
        ESP.restart();
    }
    debugPrint("\nConnected to WiFi", true);
    wifiConnected = true;
    currentLedState = WAITING_FOR_INPUT;
    vTaskDelete(NULL);
}

// Fetches current date and time from a web API
String getCurrentTimeDate() {
    if (WiFi.status() != WL_CONNECTED) {
        debugPrint("WiFi not connected");
        return "";
    }

    HTTPClient http;
    http.begin("http://worldtimeapi.org/api/ip");
    http.setTimeout(10000); // Set timeout to 10 seconds
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
        String payload = http.getString();
        debugPrint("Raw time API response:");
        debugPrint(payload);
        
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
            debugPrint("JSON parsing failed: ");
            debugPrint(error.c_str());
            http.end();
            return "";
        }
        String datetime = doc["datetime"].as<String>();
        http.end();
        debugPrint("Capacity used: " + String(doc.memoryUsage()));
        return datetime;
    } else {
        debugPrint("Error code: " + String(httpResponseCode));
        debugPrint("Response: " + http.getString());
        http.end();
        return "";
    }
}

// Queries Claude API for a Socratic quote based on the current date and time
String getQuoteFromClaude(const String& timeDate) {
    if (WiFi.status() != WL_CONNECTED) {
        debugPrint("WiFi not connected");
        return "Error: WiFi not connected";
    }

    HTTPClient http;
    http.begin(apiEndpoint);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", apiKey);
    http.addHeader("anthropic-version", "2023-06-01");

    char message[256];
    snprintf(message, sizeof(message), MESSAGE_TEMPLATE, timeDate.c_str());

    // Create a JSON object for the payload
    DynamicJsonDocument doc(1024);
    doc["model"] = CLAUDE_MODEL;
    doc["max_tokens"] = CLAUDE_MAX_TOKENS;
    doc["temperature"] = CLAUDE_TEMPERATURE;
    doc["system"] = SYSTEM_PROMPT;
    JsonArray messages = doc["messages"].to<JsonArray>();
    JsonObject messageObj = messages.createNestedObject();
    messageObj["role"] = CLAUDE_ROLE;
    messageObj["content"] = message;

    String payload;
    serializeJson(doc, payload);

    debugPrint("Sending request to Claude API...");
    debugPrint(payload);

    unsigned long startTime = millis();

    http.setTimeout(30000);  // Set timeout to 30 seconds
    int httpResponseCode = http.POST(payload);
    String response = "";

    if (httpResponseCode > 0) {
        String rawResponse = http.getString();
        debugPrint("Raw response from Claude API:");
        debugPrint(rawResponse);

        DynamicJsonDocument responseDoc(2048);
        DeserializationError error = deserializeJson(responseDoc, rawResponse);

        if (error) {
            debugPrint("deserializeJson() failed: " + String(error.c_str()));
            response = "Error: JSON parsing failed";
        } else {
            if (responseDoc["content"][0]["text"].is<String>()) {
                response = responseDoc["content"][0]["text"].as<String>();
                response.trim();
            } else {
                response = "Error: Unexpected JSON structure";
            }
        }
        debugPrint("Capacity used: " + String(responseDoc.memoryUsage()));
    } else {
        debugPrint("HTTP Error code: " + String(httpResponseCode));
        debugPrint("Response: " + http.getString());
        response = "Error: Unable to get quote";
    }

    http.end();

    unsigned long duration = millis() - startTime;
    debugPrint("API request duration: " + String(duration) + "ms");

    return response;
}

// FreeRTOS task for LED control
void ledTask(void * parameter) {
    uint8_t hue = 0;
    bool ledOn = false;
    unsigned long lastToggle = 0;
    const unsigned long blinkInterval = 500; // 0.5 second blink interval

    for(;;) {
        switch (currentLedState) {
            case WAITING_FOR_WIFI:
                // Blue pulsing effect for WiFi connection
                leds[0] = CHSV(160, 255, abs(sin(millis() / 500.0) * 255));
                FastLED.show();
                break;

            case WAITING_FOR_INPUT:
                if (millis() - lastToggle >= blinkInterval) {
                    ledOn = !ledOn;
                    leds[0] = ledOn ? CRGB::Red : CRGB::Black;
                    FastLED.show();
                    lastToggle = millis();
                }
                break;
            
            case WAITING_FOR_API:
                leds[0] = CHSV(hue, 255, 255);
                FastLED.show();
                hue++;
                break;
            
            case IDLE:
                leds[0] = CRGB::Black;
                FastLED.show();
                break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz update rate
    }
}

// Prints debug messages to Serial if DEBUG_MODE is enabled and Serial is available
void debugPrint(const String& message, bool always) {
    if ((DEBUG_MODE || always) && serialAvailable) {
        Serial.println(message);
    }
}