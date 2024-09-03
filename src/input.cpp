#include "input.h"
#include "config.h"

static unsigned long lastDebounceTime = 0;
static const unsigned long debounceDelay = 50;

void setupButton() {
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    
    // Setup prompt selector pins
    pinMode(PIN_PROMPT_SELECTOR_0, INPUT_PULLUP);
    pinMode(PIN_PROMPT_SELECTOR_1, INPUT_PULLUP);
    pinMode(PIN_PROMPT_SELECTOR_2, INPUT_PULLUP);
    pinMode(PIN_PROMPT_SELECTOR_3, INPUT_PULLUP);
    pinMode(PIN_PROMPT_SELECTOR_4, INPUT_PULLUP);
    pinMode(PIN_PROMPT_SELECTOR_5, INPUT_PULLUP);
    
    Serial.println("Button (PIN " + String(PIN_BUTTON) + ") and selector pins initialized");
}

bool isButtonPressed() {
    bool reading = digitalRead(PIN_BUTTON) == LOW;
    
    if (reading && ((millis() - lastDebounceTime) > debounceDelay)) {
        lastDebounceTime = millis();
        Serial.println("Button pressed (PIN " + String(PIN_BUTTON) + ")");
        return true;
    }
    
    return false;
}

int readPromptSelector() {
    if (digitalRead(PIN_PROMPT_SELECTOR_0) == LOW) return 1;
    if (digitalRead(PIN_PROMPT_SELECTOR_1) == LOW) return 2;
    if (digitalRead(PIN_PROMPT_SELECTOR_2) == LOW) return 3;
    if (digitalRead(PIN_PROMPT_SELECTOR_3) == LOW) return 4;
    if (digitalRead(PIN_PROMPT_SELECTOR_4) == LOW) return 5;
    if (digitalRead(PIN_PROMPT_SELECTOR_5) == LOW) return 6;
    return 1; // Default to 1 if no selector is active
}

int getSystemPromptSelector() {
    int selector = readPromptSelector();
    Serial.println("Current selector state: " + String(selector));
    return selector;
}