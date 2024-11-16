#include "feedback_display.h"

FeedbackDisplay::FeedbackDisplay(int pin, int numPixels) : pin(pin), numPixels(numPixels) {
  neoPixels = Adafruit_NeoPixel(numPixels, pin, NEO_GRB + NEO_KHZ800);
}

void FeedbackDisplay::begin() {
  neoPixels.begin();
  neoPixels.show();
}

void FeedbackDisplay::update(WifiState state) {
  switch (state) {
    case WIFI_CONNECTING:
      // Blink blue
      neoPixels.setPixelColor(0, 0x0000FF);
      neoPixels.show();
      delay(500);
      neoPixels.clear();
      delay(500);
      break;
    case WIFI_CONNECTED:
      // Solid blue
      neoPixels.setPixelColor(0, 0x0000FF);
      neoPixels.show();
      break;
    case WIFI_ERROR:
      // Blink red
      neoPixels.setPixelColor(0, 0xFF0000);
      neoPixels.show();
      delay(500);
      neoPixels.clear();
      delay(500);
      break;
  }
}