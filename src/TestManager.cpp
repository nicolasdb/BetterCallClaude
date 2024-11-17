#include "TestManager.h"

void TestManager::runSPIFFSTest() {
    Serial.println("\n=== SPIFFS Manager Test ===");

    // Initialize SPIFFS
    Serial.println("Initializing SPIFFS...");
    if (!SPIFFSManager::begin()) {
        handleSPIFFSError("SPIFFS initialization failed");
        return;
    }
    Serial.println("SPIFFS initialized successfully");

    // Test 1: Save multiple quotes
    Serial.println("\nTest 1: Saving test quotes...");
    for (int i = 0; i < 3; i++) {
        String testQuote = "Test Quote " + String(i);
        Serial.print("Saving: ");
        Serial.println(testQuote);
        
        if (!SPIFFSManager::saveQuote(testQuote)) {
            handleSPIFFSError("Failed to save quote");
            return;
        }
        Serial.println("Save successful");
        delay(100);
    }

    // Test 2: List stored quotes
    Serial.println("\nTest 2: Listing Quotes");
    SPIFFSManager::listQuotes();

    // Test 3: Load last quote
    Serial.println("\nTest 3: Loading Last Quote");
    String lastQuote = SPIFFSManager::loadLastQuote();
    Serial.println("Last Quote:");
    Serial.println(lastQuote);

    Serial.println("\n=== SPIFFS Test Complete ===");
}

void TestManager::runAPITest() {
    Serial.println("\n=== API Client Test ===");

    // Initialize SPIFFS
    Serial.println("Initializing SPIFFS...");
    if (!SPIFFSManager::begin()) {
        handleAPIError("SPIFFS initialization failed");
        return;
    }
    Serial.println("SPIFFS initialized successfully");

    // Test API Client
    Serial.println("\nTesting API connection...");
    if (!APIClient::begin()) {
        handleAPIError("API Client initialization failed");
        return;
    }
    Serial.println("API Client initialized successfully");

    // Generate and store quotes
    Serial.println("\nGenerating and storing quotes...");
    for (int i = 0; i < 3; i++) {
        Serial.print("\nGenerating quote ");
        Serial.println(i + 1);
        
        String quote = APIClient::generateAndStoreQuote(1, 0.7, 150, i);
        if (quote.startsWith("Error:")) {
            handleAPIError(quote.c_str());
            return;
        }
        
        Serial.println("Generated quote:");
        Serial.println(quote);
        delay(2000);  // Delay between API calls
    }

    // List stored quotes
    Serial.println("\nListing stored quotes:");
    SPIFFSManager::listQuotes();

    // Read last quote
    Serial.println("\nReading last quote:");
    String lastQuote = SPIFFSManager::loadLastQuote();
    Serial.println(lastQuote);

    Serial.println("\n=== API Test Complete ===");
}

bool TestManager::initWiFi() {
    Serial.println("Initializing WiFi...");
    wifi_manager_init();
    
    Serial.println("Connecting to WiFi...");
    wifi_manager_connect();
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > 20000) {  // 20s timeout
            Serial.println("WiFi Connection Failed!");
            return false;
        }
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    return true;
}

void TestManager::printState(SystemState state) {
    Serial.print("System State: ");
    switch(state) {
        case STATE_INIT: 
            Serial.println("Initializing"); 
            break;
        case STATE_WAIT_BUTTON: 
            Serial.println("Waiting for Button"); 
            break;
        case STATE_WIFI_CONNECT: 
            Serial.println("Connecting to WiFi"); 
            break;
        case STATE_TEST_RUNNING: 
            Serial.println("Running Test"); 
            break;
        case STATE_TEST_COMPLETE: 
            Serial.println("Test Complete"); 
            break;
        case STATE_ERROR: 
            Serial.println("Error"); 
            break;
    }
}

void TestManager::handleSPIFFSError(const char* message) {
    Serial.print("SPIFFS Error: ");
    Serial.println(message);
}

void TestManager::handleAPIError(const char* message) {
    Serial.print("API Error: ");
    Serial.println(message);
}
