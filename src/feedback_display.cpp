#include "feedback_display.h"

FeedbackDisplay::FeedbackDisplay(int pin, int numPixels) 
    : neoPixels(numPixels, pin, NEO_GRB + NEO_KHZ800),
      currentEffect(LED_OFF),
      currentColor(0),
      breatheValue(0),
      breatheUp(true),
      lastEffectTime(0),
      blinkCount(0),
      maxBlinks(0),
      blinkState(false) {
}

void FeedbackDisplay::begin() {
    neoPixels.begin();
    neoPixels.setBrightness(50);
    neoPixels.show();
}

void FeedbackDisplay::setEffect(LEDEffect effect, uint32_t color, int maxBlinks) {
    currentEffect = effect;
    currentColor = color;
    this->maxBlinks = maxBlinks;
    blinkCount = 0;
    blinkState = false;
    breatheValue = 0;
    breatheUp = true;
}

void FeedbackDisplay::update() {
    unsigned long currentMillis = millis();
    
    switch(currentEffect) {
        case LED_OFF:
            clear();
            break;
            
        case LED_STATIC:
            updateStatic();
            break;
            
        case LED_BLINK:
            if (currentMillis - lastEffectTime >= 100) {
                lastEffectTime = currentMillis;
                updateBlink();
            }
            break;
            
        case LED_BREATHE:
            if (currentMillis - lastEffectTime >= 20) {
                lastEffectTime = currentMillis;
                updateBreathe();
            }
            break;
    }
}

void FeedbackDisplay::updateBreathe() {
    if (breatheUp) {
        breatheValue++;
        if (breatheValue >= 100) breatheUp = false;
    } else {
        breatheValue--;
        if (breatheValue <= 0) breatheUp = true;
    }
    neoPixels.setBrightness(breatheValue);
    neoPixels.setPixelColor(0, currentColor);
    neoPixels.show();
}

void FeedbackDisplay::updateBlink() {
    blinkState = !blinkState;
    if (blinkState) {
        neoPixels.setPixelColor(0, currentColor);
    } else {
        neoPixels.clear();
    }
    neoPixels.show();
    
    if (!blinkState) {
        blinkCount++;
        if (blinkCount >= maxBlinks && maxBlinks > 0) {
            currentEffect = LED_BREATHE;
            blinkCount = 0;
        }
    }
}

void FeedbackDisplay::updateStatic() {
    neoPixels.setPixelColor(0, currentColor);
    neoPixels.show();
}

void FeedbackDisplay::setBrightness(uint8_t brightness) {
    neoPixels.setBrightness(brightness);
    neoPixels.show();
}

void FeedbackDisplay::clear() {
    neoPixels.clear();
    neoPixels.show();
}
