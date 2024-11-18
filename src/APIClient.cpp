#include "APIClient.h"
#include "wifi_manager.h"
#include "SPIFFSManager.h"
#include "json_parser.h"

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
            return SYSTEM_PROMPT_2;
        default:
            return SYSTEM_PROMPT_1;
    }
}

String APIClient::formatMessageContent() {
    return "Generate quote. Only include the final <quote> tags with the quote itself.";
}

String APIClient::generateQuote(int promptSelector, float temperature, int maxTokens) {
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
    doc["max_tokens"] = maxTokens;
    doc["temperature"] = temperature;
    
    doc["system"] = selectSystemPrompt(promptSelector);
    
    JsonArray messages = doc["messages"].to<JsonArray>();
    JsonObject messageObj = messages.createNestedObject();
    messageObj["role"] = "user";
    messageObj["content"] = formatMessageContent();

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
        
        if (httpResponseCode == 200) {
            DynamicJsonDocument responseDoc(8192);  // Increased size for larger response
            DeserializationError error = deserializeJson(responseDoc, rawResponse);

            if (error) {
                Serial.println("deserializeJson() failed: " + String(error.c_str()));
                response = "Error: JSON parsing failed";
                http.end();  // Close connection on error
            } else {
                if (responseDoc["content"][0]["text"].is<String>()) {
                    String text = responseDoc["content"][0]["text"].as<String>();
                    text.trim();

                    // Check if response contains quote tags before returning
                    String quote = JsonParser::extractQuote(text);
                    if (quote.length() > 0) {
                        response = text;
                    } else {
                        Serial.println("Response does not contain quote tags:");
                        Serial.println(text);
                        response = "Error: No quote found in response";
                    }
                } else {
                    response = "Error: Unexpected JSON structure";
                }
            }
        } else {
            Serial.printf("HTTP Error code: %d\n", httpResponseCode);
            Serial.println("Error response: " + rawResponse);
            response = "Error: API request failed with code " + String(httpResponseCode);
        }
    } else {
        Serial.println("HTTP Error code: " + String(httpResponseCode));
        Serial.println("Error response: " + http.getString());
        response = "Error: Unable to get quote";
    }

    if (!response.startsWith("Error:")) {
        lastRequestTime = millis();
    } else {
        http.end();  // Close connection on error
    }
    return response;
}

String APIClient::generateAndStoreQuote(int promptSelector, float temperature, 
                                      int maxTokens) {
    String response = generateQuote(promptSelector, temperature, maxTokens);
    
    // Only store if it's a valid quote (not an error)
    if (!response.startsWith("Error:")) {
        String quote = JsonParser::extractQuote(response);
        if (quote.length() > 0) {
            if (SPIFFSManager::saveQuote(response)) {
                if (DEBUG_MODE) {
                    Serial.println("Quote successfully stored in SPIFFS");
                }
            } else {
                Serial.println("Failed to store quote in SPIFFS");
            }
        } else {
            Serial.println("Not storing response - no valid quote found");
        }
    }
    
    return response;
}
