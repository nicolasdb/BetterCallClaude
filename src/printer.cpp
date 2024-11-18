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
    
    Serial.println("Original text:");
    Serial.println(quote);
    
    // Wrap text to printer width
    String wrappedText = _wrapText(quote, THERMAL_PRINTER_MAX_CHAR_PER_LINE);
    
    Serial.println("Wrapped text:");
    Serial.println(wrappedText);
    
    // Send the wrapped text
    THERMAL_PRINTER_SERIAL.println(wrappedText);
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
    String currentLine;
    String currentWord;
    
    // Process each character
    for (unsigned int i = 0; i < text.length(); i++) {
        char c = text[i];
        
        if (c == ' ' || c == '\n') {
            // If adding this word would exceed line length
            if (currentLine.length() + currentWord.length() + 1 > maxLineLength) {
                // Add current line to wrapped text and start new line
                wrappedText += currentLine + '\n';
                currentLine = currentWord + c;
            } else {
                // Add word to current line
                if (currentLine.length() > 0) {
                    currentLine += ' ';
                }
                currentLine += currentWord;
                if (c == '\n') {
                    wrappedText += currentLine + '\n';
                    currentLine = "";
                } else {
                    currentLine += ' ';
                }
            }
            currentWord = "";
        } else {
            currentWord += c;
            
            // If current word is already too long for a line
            if (currentWord.length() >= maxLineLength) {
                // If there's content in current line, add it first
                if (currentLine.length() > 0) {
                    wrappedText += currentLine + '\n';
                    currentLine = "";
                }
                // Add the long word with a break
                wrappedText += currentWord + '\n';
                currentWord = "";
            }
        }
    }
    
    // Handle any remaining text
    if (currentWord.length() > 0) {
        if (currentLine.length() + currentWord.length() + 1 > maxLineLength) {
            wrappedText += currentLine + '\n' + currentWord;
        } else {
            if (currentLine.length() > 0) {
                currentLine += ' ';
            }
            wrappedText += currentLine + currentWord;
        }
    } else if (currentLine.length() > 0) {
        wrappedText += currentLine;
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
