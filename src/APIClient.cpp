#include "APIClient.h"
#include "wifi_manager.h"
#include "SPIFFSManager.h"

HTTPClient APIClient::http;
unsigned long APIClient::requestDelay = 2000;  // Default 2s delay
unsigned long APIClient::lastRequestTime = 0;

bool APIClient::begin() {
    return WiFi.status() == WL_CONNECTED;
}

String APIClient::selectSystemPrompt(int promptSelector) {
    switch (promptSelector) {
        case 1:
            return "You are a wise philosopher in the style of Socrates. Generate a profound, thought-provoking quote that challenges conventional thinking.";
        case 2:
            return "Create a quote that reflects deep wisdom about human nature, knowledge, or personal growth.";
        case 3:
            return "Craft a quote that explores the relationship between individual experience and universal truth.";
        default:
            return "You are a philosopher generating insightful quotes about life and wisdom.";
    }
}

String APIClient::formatMessageContent(int randomSeed) {
    char messageContent[256];
    snprintf(messageContent, sizeof(messageContent), 
             "Generate a philosophical quote that provides insight into human nature. Use the random seed %d to inspire unique creativity.", 
             randomSeed);
    return String(messageContent);
}

String APIClient::generateQuote(int promptSelector, float temperature, int maxTokens, int randomSeed) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        return "Error: WiFi not connected";
    }

    // Check if we need to wait between requests
    unsigned long currentTime = millis();
    if (lastRequestTime > 0 && currentTime - lastRequestTime < requestDelay) {
        unsigned long waitTime = requestDelay - (currentTime - lastRequestTime);
        Serial.printf("Waiting %lums before next request...\n", waitTime);
        delay(waitTime);
    }

    // Begin HTTPS connection (reuse existing if possible)
    if (!http.connected()) {
        Serial.println("Establishing new API connection...");
        http.begin(get_api_endpoint());
        http.addHeader("Content-Type", "application/json");
        http.addHeader("x-api-key", get_api_key());
        http.addHeader("anthropic-version", "2023-06-01");
    } else {
        Serial.println("Reusing existing API connection...");
    }

    DynamicJsonDocument doc(4096);
    doc["model"] = "claude-3-5-haiku-20241022";
    doc["max_tokens"] = maxTokens;
    doc["temperature"] = temperature;
    
    doc["system"] = selectSystemPrompt(promptSelector);
    
    JsonArray messages = doc["messages"].to<JsonArray>();
    JsonObject messageObj = messages.createNestedObject();
    messageObj["role"] = "user";
    messageObj["content"] = formatMessageContent(randomSeed);

    String payload;
    serializeJson(doc, payload);
    
    if (DEBUG_MODE) {
        Serial.println("Sending payload to API:");
        Serial.println(payload);
    }

    Serial.println("Sending request to API...");
    unsigned long requestStart = millis();
    
    http.setTimeout(30000);
    int httpResponseCode = http.POST(payload);
    String response = "";

    if (httpResponseCode > 0) {
        unsigned long responseTime = millis() - requestStart;
        Serial.printf("Response received in %lums\n", responseTime);
        
        String rawResponse = http.getString();
        
        if (DEBUG_MODE) {
            Serial.println("Raw API response:");
            Serial.println(rawResponse);
        }
        
        DynamicJsonDocument responseDoc(4096);
        DeserializationError error = deserializeJson(responseDoc, rawResponse);

        if (error) {
            Serial.println("deserializeJson() failed: " + String(error.c_str()));
            response = "Error: JSON parsing failed";
            http.end();  // Close connection on error
        } else {
            if (responseDoc["content"][0]["text"].is<String>()) {
                response = responseDoc["content"][0]["text"].as<String>();
                response.trim();
            } else {
                response = "Error: Unexpected JSON structure";
                http.end();  // Close connection on error
            }
        }
    } else {
        Serial.println("HTTP Error code: " + String(httpResponseCode));
        Serial.println("Error response: " + http.getString());
        response = "Error: Unable to get quote";
        http.end();  // Close connection on error
    }

    lastRequestTime = millis();
    return response;
}

String APIClient::generateAndStoreQuote(int promptSelector, float temperature, 
                                      int maxTokens, int randomSeed) {
    String quote = generateQuote(promptSelector, temperature, maxTokens, randomSeed);
    
    if (!quote.startsWith("Error:")) {
        if (SPIFFSManager::saveQuote(quote)) {
            if (DEBUG_MODE) {
                Serial.println("Quote successfully stored in SPIFFS");
            }
        } else {
            Serial.println("Failed to store quote in SPIFFS");
        }
    }
    
    return quote;
}
