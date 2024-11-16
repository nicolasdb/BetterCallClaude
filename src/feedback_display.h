#ifndef FEEDBACK_DISPLAY_H
#define FEEDBACK_DISPLAY_H

#include <Adafruit_NeoPixel.h>

enum WifiState {
  WIFI_CONNECTING,
  WIFI_CONNECTED,
  WIFI_ERROR
};

class FeedbackDisplay {
public:
  FeedbackDisplay(int pin, int numPixels);
  void begin();
  void update(WifiState state);

private:
  Adafruit_NeoPixel neoPixels;
  int pin;
  int numPixels;
};

#endif  // FEEDBACK_DISPLAY_H