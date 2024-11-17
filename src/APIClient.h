#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

class APIClient {
public:
    static bool begin();
    static String generateAndStoreQuote(int promptSelector = 1, 
                                      float temperature = 0.7, 
                                      int maxTokens = 150, 
                                      int randomSeed = 0);
    static void setRequestDelay(unsigned long delay) { requestDelay = delay; }
    static unsigned long getLastRequestTime() { return lastRequestTime; }
    static HTTPClient* getHttpClient() { return &http; }

private:
    static String selectSystemPrompt(int promptSelector);
    static String formatMessageContent(int randomSeed);
    static String generateQuote(int promptSelector, float temperature, 
                              int maxTokens, int randomSeed);
    static HTTPClient http;
    static unsigned long requestDelay;  // Delay between requests in ms
    static unsigned long lastRequestTime;
};

#endif // API_CLIENT_H
