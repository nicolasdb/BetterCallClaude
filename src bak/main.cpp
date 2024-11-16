#include <Arduino.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "config.h"
#include "input.h"
#include "process.h"
#include "output.h"

#define FIRMWARE_VERSION "1.3.5"
#define PRINTER_ID_ADDRESS 0
#define PRINTER_ID_LENGTH 16
#define DEBUG_VERBOSE 0  // Set to 1 for verbose debug output

// FreeRTOS task handles
TaskHandle_t wifiTaskHandle;
TaskHandle_t ledTaskHandle;
TaskHandle_t buttonTaskHandle;
TaskHandle_t apiTaskHandle;
TaskHandle_t printerTaskHandle;
TaskHandle_t potentiometerTaskHandle;

// FreeRTOS queues and semaphores
QueueHandle_t apiRequestQueue;
QueueHandle_t printQueue;
SemaphoreHandle_t wifiSemaphore;

// Global variables
bool wifiConnected = false;
LedState currentLedState = WAITING_FOR_WIFI;
char printerId[PRINTER_ID_LENGTH + 1];  // +1 for null terminator

// Function prototypes
void generatePrinterId();
void loadOrGeneratePrinterId();
void wifiTask(void *pvParameters);
void ledTask(void *pvParameters);
void buttonTask(void *pvParameters);
void apiTask(void *pvParameters);
void printerTask(void *pvParameters);
void potentiometerTask(void *pvParameters);

// External function declarations
extern void setupInput();
extern void setupLED();
extern void setupThermalPrinter();
extern void setupWiFi();
extern bool isButtonPressed();
extern bool isButtonReleased();
extern int getSystemPromptSelector();
extern float getClaudeTemperature();
extern int getClaudeMaxTokens();
extern int getClaudeRandomSeed();
extern void updateLED(LedState state);
extern String getQuoteFromClaude(int promptSelector, float temperature, int maxTokens, int randomSeed);
extern void printQuote(const String& quote);
extern void readAndPrintPotentiometers();

// ApiParams struct definition
struct ApiParams {
    int promptSelector;
    float temperature;
    int maxTokens;
    int randomSeed;
};

void setup() {
    Serial.begin(115200);
    delay(1000);  // Give some time for the serial connection to establish
    Serial.println("\n\n--- BetterCallClaude Printer Starting ---");
    Serial.printf("Firmware version: %s\n", FIRMWARE_VERSION);
    
    loadOrGeneratePrinterId();
    setupInput();
    setupLED();
    setupThermalPrinter();
    
    // Create FreeRTOS objects
    apiRequestQueue = xQueueCreate(5, sizeof(ApiParams));
    printQueue = xQueueCreate(5, sizeof(String));
    wifiSemaphore = xSemaphoreCreateBinary();
    
    // Create FreeRTOS tasks
    xTaskCreatePinnedToCore(wifiTask, "WiFiTask", 4096, NULL, 1, &wifiTaskHandle, 0);
    xTaskCreatePinnedToCore(ledTask, "LEDTask", 2048, NULL, 2, &ledTaskHandle, 1);
    xTaskCreatePinnedToCore(buttonTask, "ButtonTask", 4096, NULL, 2, &buttonTaskHandle, 1);
    xTaskCreatePinnedToCore(apiTask, "APITask", 8192, NULL, 1, &apiTaskHandle, 0);
    xTaskCreatePinnedToCore(printerTask, "PrinterTask", 4096, NULL, 1, &printerTaskHandle, 1);
    xTaskCreatePinnedToCore(potentiometerTask, "PotentiometerTask", 2048, NULL, 1, &potentiometerTaskHandle, 1);
    
    Serial.println("Setup completed. FreeRTOS scheduler starting...");
}

void loop() {
    // Empty. Tasks are handled by FreeRTOS
    vTaskDelete(NULL);
}

void generatePrinterId() {
    randomSeed(analogRead(0));
    for (int i = 0; i < PRINTER_ID_LENGTH; i++) {
        printerId[i] = (i % 2 == 0) ? random(48, 58) : random(97, 123);
    }
    printerId[PRINTER_ID_LENGTH] = '\0';
    Serial.printf("New Printer ID generated: %s\n", printerId);
}

void loadOrGeneratePrinterId() {
    EEPROM.begin(PRINTER_ID_LENGTH);
    bool shouldGenerateNewId = false;

    for (int i = 0; i < PRINTER_ID_LENGTH; i++) {
        char c = EEPROM.read(PRINTER_ID_ADDRESS + i);
        if (c == 255) {
            shouldGenerateNewId = true;
            break;
        }
        printerId[i] = c;
    }
    printerId[PRINTER_ID_LENGTH] = '\0';

    if (shouldGenerateNewId) {
        generatePrinterId();
        for (int i = 0; i < PRINTER_ID_LENGTH; i++) {
            EEPROM.write(PRINTER_ID_ADDRESS + i, printerId[i]);
        }
        EEPROM.commit();
    }

    Serial.printf("Printer ID: %s\n", printerId);
}

void wifiTask(void *pvParameters) {
    setupWiFi();
    for (;;) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected. Attempting to reconnect...");
            WiFi.reconnect();
            vTaskDelay(pdMS_TO_TICKS(5000));
        } else if (!wifiConnected) {
            wifiConnected = true;
            xSemaphoreGive(wifiSemaphore);
            currentLedState = WAITING_FOR_INPUT;
            Serial.println("WiFi connected. LED state set to WAITING_FOR_INPUT.");
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void ledTask(void *pvParameters) {
    for (;;) {
        updateLED(currentLedState);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void buttonTask(void *pvParameters) {
    for (;;) {
        if (isButtonPressed() && wifiConnected) {
            Serial.println("Button pressed (PIN 10)");
            currentLedState = WAITING_FOR_API;
            
            while (!isButtonReleased()) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            
            Serial.println("Button released (PIN 10)");
            ApiParams apiParams;
            apiParams.promptSelector = getSystemPromptSelector();
            apiParams.temperature = getClaudeTemperature();
            apiParams.maxTokens = getClaudeMaxTokens();
            apiParams.randomSeed = getClaudeRandomSeed();
            
            readAndPrintPotentiometers();
            
            Serial.printf("Prompt selector: %d, Temperature: %.2f, Max Tokens: %d, Random Seed: %d\n", 
                          apiParams.promptSelector, apiParams.temperature, apiParams.maxTokens, apiParams.randomSeed);
            
            if (xQueueSendToBack(apiRequestQueue, &apiParams, 0) != pdPASS) {
                Serial.println("Failed to send API parameters to queue. Queue might be full.");
                currentLedState = WAITING_FOR_INPUT;
            } else {
                Serial.println("API parameters sent to queue successfully.");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void apiTask(void *pvParameters) {
    ApiParams apiParams;
    for (;;) {
        if (xQueueReceive(apiRequestQueue, &apiParams, portMAX_DELAY) == pdTRUE) {
            Serial.printf("Received API parameters: Prompt selector: %d, Temperature: %.2f, Max Tokens: %d, Random Seed: %d\n", 
                          apiParams.promptSelector, apiParams.temperature, apiParams.maxTokens, apiParams.randomSeed);
            Serial.println("Requesting quote from Claude...");
            String quote = getQuoteFromClaude(apiParams.promptSelector, apiParams.temperature, apiParams.maxTokens, apiParams.randomSeed);
            if (quote != "") {
                currentLedState = IDLE;
                Serial.println("Quote received. Sending to print queue.");
                if (xQueueSendToBack(printQueue, &quote, 0) != pdPASS) {
                    Serial.println("Failed to send quote to print queue. Queue might be full.");
                } else {
                    Serial.println("Quote sent to print queue successfully.");
                }
            } else {
                Serial.println("Failed to get quote from Claude.");
            }
            currentLedState = WAITING_FOR_INPUT;
        }
    }
}

void printerTask(void *pvParameters) {
    String quoteToPrint;
    for (;;) {
        if (xQueueReceive(printQueue, &quoteToPrint, portMAX_DELAY) == pdTRUE) {
            Serial.println("Quote received from print queue. Printing...");
            printQuote(quoteToPrint);
            Serial.println("Quote printed.");
        }
    }
}

void potentiometerTask(void *pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1 second interval

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // Wait for the next cycle.
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Perform the potentiometer reading
        readAndPrintPotentiometers();
    }
}