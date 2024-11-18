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
            return SYSTEM_PROMPT_1;
        case 2:
            // TODO: Return SYSTEM_PROMPT_2 when defined
            return "Create a quote that reflects deep wisdom about human nature, knowledge, or personal growth.";
        default:
            return SYSTEM_PROMPT_1;
    }
}

String APIClient::formatMessageContent(int randomSeed) {
    // Request only the final quote without the idea generation process
    return "Generate a quote. Only include the final <quote> tags with the quote itself, not the idea generation process.";
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

    DynamicJsonDocument doc(8192);  // Increased size for larger prompts
    doc["model"] = "claude-3-haiku-20240307"; 
    doc["max_tokens"] = 1000;  // Increased to ensure we get complete response
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
        
        DynamicJsonDocument responseDoc(8192);  // Increased size for larger response
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
