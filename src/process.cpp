#include "process.h"
#include "config.h"
#include "credentials.h"
#include "input.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

extern bool wifiConnected;
extern SemaphoreHandle_t wifiSemaphore;

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
    xSemaphoreGive(wifiSemaphore);
}

String getQuoteFromClaude(int promptSelector) {
    if (xSemaphoreTake(wifiSemaphore, pdMS_TO_TICKS(5000)) != pdTRUE) {
        Serial.println("Failed to acquire WiFi semaphore");
        return "Error: WiFi not available";
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        xSemaphoreGive(wifiSemaphore);
        return "Error: WiFi not connected";
    }

    HTTPClient http;
    http.begin(apiEndpoint);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", apiKey);
    http.addHeader("anthropic-version", "2023-06-01");

    DynamicJsonDocument doc(4096);
    doc["model"] = CLAUDE_MODEL;
    doc["max_tokens"] = CLAUDE_MAX_TOKENS;
    doc["temperature"] = CLAUDE_TEMPERATURE;
    
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
    messageObj["role"] = "user";
    messageObj["content"] = MESSAGE_TEMPLATE;

    String payload;
    serializeJson(doc, payload);
    
    Serial.println("Sending payload to API:");
    Serial.println(payload);

    http.setTimeout(30000);
    int httpResponseCode = http.POST(payload);
    String response = "";

    if (httpResponseCode > 0) {
        String rawResponse = http.getString();
        Serial.println("Raw API response:");
        Serial.println(rawResponse);
        
        DynamicJsonDocument responseDoc(4096);
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
        Serial.println("Error response: " + http.getString());
        response = "Error: Unable to get quote";
    }

    http.end();
    xSemaphoreGive(wifiSemaphore);
    return response;
}