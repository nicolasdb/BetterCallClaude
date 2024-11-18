#include "printer.h"
#include "config.h"
#include <Arduino.h>

// Standard ESC/POS commands
#define ESC 0x1B
#define GS  0x1D

Printer::Printer() : _currentStatus(IDLE) {}

bool Printer::begin() {
    Serial.println("\n=== Initializing Printer ===");
    
    // Initialize serial communication
    Serial.printf("Configuring Serial1 - RX: %d, TX: %d, Baud: %d\n", 
                 THERMAL_PRINTER_RX, THERMAL_PRINTER_TX, THERMAL_PRINTER_BAUD_RATE);
                 
    THERMAL_PRINTER_SERIAL.begin(THERMAL_PRINTER_BAUD_RATE, SERIAL_8N1, THERMAL_PRINTER_RX, THERMAL_PRINTER_TX);
    delay(500);  // Wait for serial to initialize
    
    Serial.println("Serial1 initialized");
    
    // Test serial communication
    Serial.println("Testing printer communication...");
    THERMAL_PRINTER_SERIAL.write('\n');  // Send a newline
    delay(100);
    
    // Send initialization sequence
    Serial.println("Sending printer init sequence...");
    _currentStatus = _initializePrinter() ? IDLE : ERROR_COMMUNICATION;
    
    Serial.printf("Printer initialization %s\n", 
                 _currentStatus == IDLE ? "successful" : "failed");
    Serial.printf("Current status: %s\n", getStatusMessage().c_str());
    
    return _currentStatus == IDLE;
}

bool Printer::_initializePrinter() {
    // Send initialization sequence
    THERMAL_PRINTER_SERIAL.write(ESC);
    THERMAL_PRINTER_SERIAL.write('@');     // Initialize printer
    delay(100);
    
    // Send a test character
    THERMAL_PRINTER_SERIAL.write('.');
    delay(100);
    
    return true;
}

bool Printer::printQuote(const String& quote) {
    Serial.println("\n=== Printing Quote ===");
    Serial.printf("Current status: %s\n", getStatusMessage().c_str());
    
    if (_currentStatus != IDLE) {
        Serial.println("Printer not ready to print");
        return false;
    }

    _updateStatus(PRINTING);
    
    Serial.println("Sending data to printer:");
    Serial.println(quote);
    
    // Send the data
    THERMAL_PRINTER_SERIAL.println(quote);
    delay(100);  // Wait for printing
    
    // Feed paper
    THERMAL_PRINTER_SERIAL.write('\n');
    THERMAL_PRINTER_SERIAL.write('\n');
    delay(100);
    
    _updateStatus(IDLE);
    Serial.println("Print operation complete");
    
    return true;
}

String Printer::_wrapText(const String& text, int maxLineLength) {
    String wrappedText;
    int lineLength = 0;
    
    for (unsigned int i = 0; i < text.length(); i++) {
        if (lineLength >= maxLineLength) {
            wrappedText += '\n';
            lineLength = 0;
        }
        wrappedText += text[i];
        lineLength++;
        if (text[i] == '\n') {
            lineLength = 0;
        }
    }
    
    return wrappedText;
}

bool Printer::_sendToPrinter(const String& data) {
    THERMAL_PRINTER_SERIAL.println(data);
    return true;
}

Printer::PrinterStatus Printer::getStatus() {
    return _currentStatus;
}

String Printer::getStatusMessage() {
    switch (_currentStatus) {
        case IDLE: return "Printer Ready";
        case PRINTING: return "Printing in Progress";
        case ERROR_PAPER_JAM: return "Error: Paper Jam";
        case ERROR_NO_PAPER: return "Error: No Paper";
        case ERROR_COMMUNICATION: return "Error: Communication Failure";
        default: return "Unknown Status";
    }
}

void Printer::_updateStatus(PrinterStatus status) {
    _currentStatus = status;
    Serial.printf("Printer status updated: %s\n", getStatusMessage().c_str());
}
