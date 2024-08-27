#include "process.h"
#include "config.h"
#include "credentials.h"
#include "input.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

extern bool wifiConnected;

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
}

String getCurrentTimeDate() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        return "";
    }

    HTTPClient http;
    http.begin("http://worldtimeapi.org/api/ip");
    http.setTimeout(10000);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
        String payload = http.getString();
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
            Serial.println("JSON parsing failed: " + String(error.c_str()));
            http.end();
            return "";
        }
        String datetime = doc["datetime"].as<String>();
        http.end();
        return datetime;
    } else {
        Serial.println("Error code: " + String(httpResponseCode));
        http.end();
        return "";
    }
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
    doc["system"] = promptSelector == 0 ? SYSTEM_PROMPT_0 : SYSTEM_PROMPT_1;
    
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