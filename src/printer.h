#ifndef PRINTER_H
#define PRINTER_H

#include <Arduino.h>

class Printer {
public:
    enum PrinterStatus {
        IDLE,
        PRINTING,
        ERROR_PAPER_JAM,
        ERROR_NO_PAPER,
        ERROR_COMMUNICATION
    };

    Printer();
    bool begin();  // Initialize printer connection
    bool printQuote(const String& quote);
    PrinterStatus getStatus();
    String getStatusMessage();

private:
    PrinterStatus _currentStatus;
    
    // Private methods for internal printer communication
    bool _initializePrinter();
    bool _sendToPrinter(const String& data);
    void _updateStatus(PrinterStatus status);
    
    // Text wrapping method to handle thermal printer line limitations
    String _wrapText(const String& text, int maxLineLength);
};

#endif // PRINTER_H
