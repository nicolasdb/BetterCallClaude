#include "input.h"
#include "config.h"

static unsigned long lastDebounceTime = 0;
static const unsigned long debounceDelay = 50;

void setupButton() {
    pinMode(PIN_BUTTON, INPUT_PULLUP);
}

bool isButtonPressed() {
    if (digitalRead(PIN_BUTTON) == LOW) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
            lastDebounceTime = millis();
            return true;
        }
    }
    return false;
}

int getSystemPromptSelector() {
    // Return the value set in config.h
    return SYSTEM_PROMPT_SELECTOR;
}