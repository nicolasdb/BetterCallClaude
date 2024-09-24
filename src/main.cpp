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

#define FIRMWARE_VERSION "1.2.2"
#define PRINTER_ID_ADDRESS 0
#define PRINTER_ID_LENGTH 16

// FreeRTOS task handles
TaskHandle_t wifiTaskHandle;
TaskHandle_t ledTaskHandle;
TaskHandle_t buttonTaskHandle;
TaskHandle_t apiTaskHandle;
TaskHandle_t printerTaskHandle;

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

// External function declarations
extern void setupButton();
extern void setupLED();
extern void setupThermalPrinter();
extern void setupWiFi();
extern bool isButtonPressed();
extern bool isButtonReleased();
extern int getSystemPromptSelector();
extern void updateLED(LedState state);
extern String getQuoteFromClaude(int promptSelector);
extern void printQuote(const String& quote);

void setup() {
    Serial.begin(115200);
    Serial.println("Starting setup...");
    Serial.print("Firmware version: ");
    Serial.println(FIRMWARE_VERSION);
    
    loadOrGeneratePrinterId();
    
    setupButton();
    setupLED();
    setupThermalPrinter();
    
    // Create FreeRTOS objects
    apiRequestQueue = xQueueCreate(5, sizeof(int));
    printQueue = xQueueCreate(5, sizeof(String));
    wifiSemaphore = xSemaphoreCreateBinary();
    
    // Create FreeRTOS tasks
    xTaskCreatePinnedToCore(wifiTask, "WiFiTask", 4096, NULL, 1, &wifiTaskHandle, 0);
    xTaskCreatePinnedToCore(ledTask, "LEDTask", 2048, NULL, 2, &ledTaskHandle, 1);
    xTaskCreatePinnedToCore(buttonTask, "ButtonTask", 4096, NULL, 2, &buttonTaskHandle, 1);
    xTaskCreatePinnedToCore(apiTask, "APITask", 8192, NULL, 1, &apiTaskHandle, 0);
    xTaskCreatePinnedToCore(printerTask, "PrinterTask", 4096, NULL, 1, &printerTaskHandle, 1);
    
    Serial.println("Setup completed. FreeRTOS scheduler starting...");
}

void loop() {
    // Empty. Tasks are handled by FreeRTOS
    vTaskDelete(NULL);
}

void generatePrinterId() {
    randomSeed(analogRead(0));
    for (int i = 0; i < PRINTER_ID_LENGTH; i++) {
        if (i % 2 == 0) {
            printerId[i] = random(48, 58);  // 0-9
        } else {
            printerId[i] = random(97, 123);  // a-z
        }
    }
    printerId[PRINTER_ID_LENGTH] = '\0';  // Null terminator
}

void loadOrGeneratePrinterId() {
    EEPROM.begin(PRINTER_ID_LENGTH);
    bool shouldGenerateNewId = false;

    for (int i = 0; i < PRINTER_ID_LENGTH; i++) {
        char c = EEPROM.read(PRINTER_ID_ADDRESS + i);
        if (c == 255) {  // Uninitialized EEPROM
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

    Serial.print("Printer ID: ");
    Serial.println(printerId);
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
        }
        vTaskDelay(pdMS_TO_TICKS(10000));  // Check WiFi status every 10 seconds
    }
}

void ledTask(void *pvParameters) {
    for (;;) {
        updateLED(currentLedState);
        vTaskDelay(pdMS_TO_TICKS(50));  // Update LED every 50ms
    }
}

void buttonTask(void *pvParameters) {
    for (;;) {
        if (isButtonPressed() && wifiConnected) {
            Serial.println("Button press detected. Waiting for release...");
            currentLedState = WAITING_FOR_API;
            
            while (!isButtonReleased()) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            
            Serial.println("Button released. Requesting quote...");
            int promptSelector = getSystemPromptSelector();
            BaseType_t xStatus = xQueueSendToBack(apiRequestQueue, &promptSelector, 0);
            if (xStatus != pdPASS) {
                Serial.println("Failed to send prompt selector to API queue. Queue might be full.");
                currentLedState = WAITING_FOR_INPUT;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // Check button every 10ms
    }
}

void apiTask(void *pvParameters) {
    int promptSelector;
    for (;;) {
        if (xQueueReceive(apiRequestQueue, &promptSelector, portMAX_DELAY) == pdTRUE) {
            String quote = getQuoteFromClaude(promptSelector);
            if (quote != "") {
                currentLedState = IDLE;
                Serial.println("Quote received. Sending to print queue.");
                BaseType_t xStatus = xQueueSendToBack(printQueue, &quote, 0);
                if (xStatus != pdPASS) {
                    Serial.println("Failed to send quote to print queue. Queue might be full.");
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
            printQuote(quoteToPrint);
        }
    }
}