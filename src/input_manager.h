#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>
#include "config.h"

class InputManager {
public:
    // Selector positions
    enum SelectorPosition {
        POSITION_0 = 0,  // print pitch.txt
        POSITION_1 = 1,  // use Prompt selection 1
        POSITION_2 = 2,  // use Prompt selection 2
        POSITION_X = -1  // out of range/disconnected position
    };

    InputManager();
    void begin();
    
    // Button methods
    bool isBuiltinButtonPressed();
    bool isExternalButtonPressed();
    
    // Selector methods
    SelectorPosition getSelectorPosition();
    
    // For future I2C sensor expansion
    bool beginI2C();
    void updateSensors();

private:
    // Pin definitions for selector
    static const uint8_t PIN_SELECTOR_0 = 8;
    static const uint8_t PIN_SELECTOR_1 = 7;
    static const uint8_t PIN_SELECTOR_2 = 6;

    // Button debouncing
    unsigned long _lastBuiltinDebounceTime = 0;
    unsigned long _lastExternalDebounceTime = 0;
    static const unsigned long DEBOUNCE_DELAY = 50;

    // Private methods
    uint8_t _readSelectorPins();
};

#endif // INPUT_MANAGER_H
