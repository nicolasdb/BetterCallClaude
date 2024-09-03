#include "process.h"
#include "config.h"
#include "credentials.h"
#include "input.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>

extern bool wifiConnected;
unsigned long bootTime = 0;

void setupWiFi() {
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nFailed to connect to WiFi. Restarting...");
        ESP.restart();
    }
    Serial.println("\nConnected to WiFi");
    wifiConnected = true;
    bootTime = millis();
}

String getLocalTime() {
    unsigned long currentTime = millis();
    unsigned long seconds = (currentTime - bootTime) / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    unsigned long days = hours / 24;

    char timeString[20];
    snprintf(timeString, sizeof(timeString), "%02lu:%02lu:%02lu", hours % 24, minutes % 60, seconds % 60);
    return String(timeString);
}

String getCurrentTimeDate() {
    const int maxRetries = 3;
    int retryCount = 0;

    while (retryCount < maxRetries) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi not connected. Attempting to reconnect...");
            WiFi.reconnect();
            delay(5000);
            if (WiFi.status() != WL_CONNECTED) {
                Serial.println("WiFi reconnection failed");
                retryCount++;
                continue;
            }
        }

        HTTPClient http;
        http.begin("http://worldtimeapi.org/api/ip");
        http.setTimeout(10000);
        int httpResponseCode = http.GET();
        
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        if (httpResponseCode > 0) {
            String payload = http.getString();
            Serial.println("Raw payload: " + payload);

            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, payload);
            if (error) {
                Serial.println("JSON parsing failed: " + String(error.c_str()));
                http.end();
                retryCount++;
                continue;
            }
            String datetime = doc["datetime"].as<String>();
            http.end();
            return datetime;
        } else {
            Serial.println("Error code: " + String(httpResponseCode));
            Serial.println("Error message: " + http.errorToString(httpResponseCode));
            http.end();
            retryCount++;
            delay(1000);  // Wait a bit before retrying
        }
    }

    Serial.println("Failed to get current time and date after " + String(maxRetries) + " attempts");
    Serial.println("Using local time as fallback");
    return getLocalTime();
}

String getQuoteFromClaude(const String& timeDate) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        return "Error: WiFi not connected";
    }

    HTTPClient http;
    http.begin(apiEndpoint);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", apiKey);
    http.addHeader("anthropic-version", "2023-06-01");

    char message[256];
    snprintf(message, sizeof(message), MESSAGE_TEMPLATE, timeDate.c_str());

    DynamicJsonDocument doc(1024);
    doc["model"] = CLAUDE_MODEL;
    doc["max_tokens"] = CLAUDE_MAX_TOKENS;
    doc["temperature"] = CLAUDE_TEMPERATURE;
    
    // Use the system prompt selector to choose the appropriate prompt
    int promptSelector = getSystemPromptSelector();
    String systemPrompt;
    switch (promptSelector) {
        case 1:
            systemPrompt = SYSTEM_PROMPT_1;
            break;
        case 2:
            systemPrompt = SYSTEM_PROMPT_2;
            break;
        case 3:
            systemPrompt = SYSTEM_PROMPT_3;
            break;
        case 4:
            systemPrompt = SYSTEM_PROMPT_4;
            break;
        case 5:
            systemPrompt = SYSTEM_PROMPT_5;
            break;
        case 6:
            systemPrompt = SYSTEM_PROMPT_6;
            break;
        default:
            systemPrompt = SYSTEM_PROMPT_1;
            break;
    }
    doc["system"] = systemPrompt;
    
    JsonArray messages = doc["messages"].to<JsonArray>();
    JsonObject messageObj = messages.createNestedObject();
    messageObj["role"] = CLAUDE_ROLE;
    messageObj["content"] = message;

    String payload;
    serializeJson(doc, payload);

    http.setTimeout(30000);
    int httpResponseCode = http.POST(payload);
    String response = "";

    if (httpResponseCode > 0) {
        String rawResponse = http.getString();
        DynamicJsonDocument responseDoc(2048);
        DeserializationError error = deserializeJson(responseDoc, rawResponse);

        if (error) {
            Serial.println("deserializeJson() failed: " + String(error.c_str()));
            response = "Error: JSON parsing failed";
        } else {
            if (responseDoc["content"][0]["text"].is<String>()) {
                response = responseDoc["content"][0]["text"].as<String>();
                response.trim();
            } else {
                response = "Error: Unexpected JSON structure";
            }
        }
    } else {
        Serial.println("HTTP Error code: " + String(httpResponseCode));
        response = "Error: Unable to get quote";
    }

    http.end();
    return response;
}