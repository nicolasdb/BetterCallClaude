#include "input.h"
#include "config.h"

static unsigned long lastDebounceTime = 0;
static const unsigned long debounceDelay = 50;
static bool lastButtonState = HIGH;
static bool buttonState = HIGH;

void setupInput() {
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    
    // Setup prompt selector pins (now only 3 pins)
    pinMode(PIN_PROMPT_SELECTOR_0, INPUT_PULLUP);
    pinMode(PIN_PROMPT_SELECTOR_1, INPUT_PULLUP);
    pinMode(PIN_PROMPT_SELECTOR_2, INPUT_PULLUP);
    
    // Setup analog inputs for potentiometers
    pinMode(PIN_POTENTIOMETER_1, INPUT);
    pinMode(PIN_POTENTIOMETER_2, INPUT);
    
    // Initialize ADC
    analogReadResolution(12);  // Set ADC resolution to 12 bits (0-4095)
    
    if (DEBUG_VERBOSE) {
        Serial.println("Button, selector pins, and potentiometers initialized");
    }
}

bool isButtonPressed() {
    bool reading = digitalRead(PIN_BUTTON);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == LOW) {
                return true;
            }
        }
    }

    lastButtonState = reading;
    return false;
}

bool isButtonReleased() {
    bool reading = digitalRead(PIN_BUTTON);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == HIGH) {
                return true;
            }
        }
    }

    lastButtonState = reading;
    return false;
}

int readPromptSelector() {
    if (digitalRead(PIN_PROMPT_SELECTOR_0) == LOW) return 1;
    if (digitalRead(PIN_PROMPT_SELECTOR_1) == LOW) return 2;
    if (digitalRead(PIN_PROMPT_SELECTOR_2) == LOW) return 3;
    return 1; // Default to 1 if no selector is active
}

int getSystemPromptSelector() {
    int selector = readPromptSelector();
    Serial.println("Current selector state: " + String(selector));
    return selector;
}

int readPotentiometer(int pin) {
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += analogRead(pin);
        delay(1);  // Small delay between readings
    }
    return sum / 10;  // Return average of 10 readings
}

float getClaudeTemperature() {
    int rawValue = readPotentiometer(PIN_POTENTIOMETER_1);
    return map(rawValue, 0, 4095, CLAUDE_TEMPERATURE_MIN * 100, CLAUDE_TEMPERATURE_MAX * 100) / 100.0;
}

int getClaudeMaxTokens() {
    int rawValue = readPotentiometer(PIN_POTENTIOMETER_2);
    return map(rawValue, 0, 4095, CLAUDE_MAX_TOKENS_MIN, CLAUDE_MAX_TOKENS_MAX);
}

void readAndPrintPotentiometers() {
    int rawTemp = readPotentiometer(PIN_POTENTIOMETER_1);
    int rawTokens = readPotentiometer(PIN_POTENTIOMETER_2);
    float temperature = getClaudeTemperature();
    int maxTokens = getClaudeMaxTokens();
    
    Serial.printf("Potentiometer 1 (PIN %d) - Raw: %d, Temperature: %.2f\n", 
                  PIN_POTENTIOMETER_1, rawTemp, temperature);
    Serial.printf("Potentiometer 2 (PIN %d) - Raw: %d, Max Tokens: %d\n", 
                  PIN_POTENTIOMETER_2, rawTokens, maxTokens);
}

int getClaudeRandomSeed() {
    return random(CLAUDE_SEED_MIN, CLAUDE_SEED_MAX + 1);
}