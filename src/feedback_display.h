#ifndef FEEDBACK_DISPLAY_H
#define FEEDBACK_DISPLAY_H

#include <Adafruit_NeoPixel.h>
#include "config.h"

enum LEDEffect {
    LED_OFF,
    LED_STATIC,
    LED_BLINK,
    LED_BREATHE
};

class FeedbackDisplay {
public:
    FeedbackDisplay(int pin, int numPixels);
    void begin();
    void setEffect(LEDEffect effect, uint32_t color = 0, int maxBlinks = 0);
    void update();
    void setBrightness(uint8_t brightness);
    void clear();

private:
    Adafruit_NeoPixel neoPixels;
    LEDEffect currentEffect;
    uint32_t currentColor;
    int breatheValue;
    bool breatheUp;
    unsigned long lastEffectTime;
    int blinkCount;
    int maxBlinks;
    bool blinkState;

    void updateBreathe();
    void updateBlink();
    void updateStatic();
};

#endif // FEEDBACK_DISPLAY_H
