#include "input_manager.h"
#include <Arduino.h>

InputManager::InputManager() {}

void InputManager::begin() {
    // Configure button pins
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_EXTERNAL_BUTTON, INPUT_PULLUP);
    
    // Configure selector pins
    pinMode(PIN_SELECTOR_0, INPUT_PULLUP);
    pinMode(PIN_SELECTOR_1, INPUT_PULLUP);
    pinMode(PIN_SELECTOR_2, INPUT_PULLUP);
}

bool InputManager::isBuiltinButtonPressed() {
    static bool lastButtonState = HIGH;
    static bool buttonState = HIGH;
    
    bool reading = digitalRead(PIN_BUTTON);
    
    if (reading != lastButtonState) {
        _lastBuiltinDebounceTime = millis();
    }
    
    if ((millis() - _lastBuiltinDebounceTime) > DEBOUNCE_DELAY) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == LOW) {  // Button is active LOW
                return true;
            }
        }
    }
    
    lastButtonState = reading;
    return false;
}

bool InputManager::isExternalButtonPressed() {
    static bool lastButtonState = HIGH;
    static bool buttonState = HIGH;
    
    bool reading = digitalRead(PIN_EXTERNAL_BUTTON);
    
    if (reading != lastButtonState) {
        _lastExternalDebounceTime = millis();
    }
    
    if ((millis() - _lastExternalDebounceTime) > DEBOUNCE_DELAY) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == LOW) {  // Button is active LOW
                return true;
            }
        }
    }
    
    lastButtonState = reading;
    return false;
}

uint8_t InputManager::_readSelectorPins() {
    uint8_t position = 0;
    
    // Read each pin and combine them into a position
    if (digitalRead(PIN_SELECTOR_0) == LOW) position |= 0x01;
    if (digitalRead(PIN_SELECTOR_1) == LOW) position |= 0x02;
    if (digitalRead(PIN_SELECTOR_2) == LOW) position |= 0x04;
    
    return position;
}

InputManager::SelectorPosition InputManager::getSelectorPosition() {
    uint8_t rawPosition = _readSelectorPins();
    
    // Convert the raw pin readings to a selector position
    switch (rawPosition) {
        case 0x01: return POSITION_0;  // Only PIN_SELECTOR_0 is active
        case 0x02: return POSITION_1;  // Only PIN_SELECTOR_1 is active
        case 0x04: return POSITION_2;  // Only PIN_SELECTOR_2 is active
        default: return POSITION_X;    // Any other combination is out of range
    }
}

bool InputManager::beginI2C() {
    // To be implemented when adding I2C sensors
    // Will initialize I2C bus and configure sensors
    return true;
}

void InputManager::updateSensors() {
    // To be implemented when adding I2C sensors
    // Will read and update sensor values
}
