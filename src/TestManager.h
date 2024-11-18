#ifndef TEST_MANAGER_H
#define TEST_MANAGER_H

#include <Arduino.h>
#include "config.h"
#include "SPIFFSManager.h"
#include "APIClient.h"
#include "wifi_manager.h"
#include "printer.h"

enum SystemState {
    STATE_INIT,
    STATE_WAIT_BUTTON,
    STATE_WIFI_CONNECT,
    STATE_TEST_RUNNING,
    STATE_TEST_COMPLETE,
    STATE_ERROR
};

class TestManager {
public:
    static void runSPIFFSTest();
    static void runAPITest();
    static bool initWiFi();
    static void printState(SystemState state);
    static void runPrinterTest();  // Added new printer test method
    
private:
    static void handleSPIFFSError(const char* message);
    static void handleAPIError(const char* message);
    static void handlePrinterError(const char* message);  // Added printer error handler
};

#endif // TEST_MANAGER_H
