# Quote Generator Testing Documentation

## Overview

This document describes the testing framework for the Quote Generator project. The project uses a modular architecture with a configurable test system integrated into the main application.

## Module Architecture

### 1. FeedbackDisplay Module
- Handles LED visual feedback
- Supports multiple effects:
  * LED_OFF: Display off
  * LED_STATIC: Solid color
  * LED_BLINK: Blinking pattern
  * LED_BREATHE: Breathing effect
- Color codes:
  * Blue breathing: Waiting for input
  * Cyan breathing: Operation in progress
  * Green blink: Success
  * Red static: Error

### 2. APIClient Module
- Manages Claude AI API communication
- Features:
  * Connection reuse for better performance
  * Configurable request delays
  * Detailed timing information
  * Error handling and recovery
- Debug output includes:
  * Connection status
  * Request timing
  * Response details

### 3. SPIFFSManager Module
- Handles quote storage
- Maintains last 5 quotes
- File operations:
  * Save quotes as JSON
  * Retrieve latest quote
  * List stored quotes
  * Auto-cleanup old quotes

### 4. TestManager Module
- Coordinates test execution
- Manages test states
- Provides error handling
- Reports test progress

## Test Configuration

In config.h:
```cpp
// Debug Mode
#define DEBUG_MODE 1  // Enable debug output

// Test Mode Selection
#define TEST_MODE_NONE 0      // Normal operation
#define TEST_MODE_SPIFFS 1    // SPIFFS test mode
#define TEST_MODE_API 2       // API test mode

// Select active test mode
#define CURRENT_TEST_MODE TEST_MODE_API  // Change as needed
```

## Test Modes

### 1. SPIFFS Test Mode
Tests storage functionality:
- SPIFFS initialization
- Quote storage
- Quote retrieval
- Storage rotation
- File management

Expected output:
```
=== SPIFFS Manager Test ===
Initializing SPIFFS...
Test 1: Saving test quotes...
Test 2: Listing Quotes...
Test 3: Loading Last Quote...
=== SPIFFS Test Complete ===
```

### 2. API Test Mode
Tests API integration:
- WiFi connection
- API communication
- Quote generation
- Storage integration

Expected output:
```
=== API Client Test ===
Initializing SPIFFS...
Testing API connection...
Generating and storing quotes...
[Timing information]
[Connection status]
=== API Test Complete ===
```

## Visual Feedback States

1. Startup:
   - Blue breathing: System ready
   - Press button to start test

2. During Test:
   - Cyan breathing: Test running
   - Green blink: Operation successful
   - Red static: Error occurred

3. Test Complete:
   - Green breathing: Test successful
   - Press button to restart

## Debugging

### Serial Output
- Baud rate: 115200
- Debug information when DEBUG_MODE = 1
- Timing and connection details
- Operation status updates

### Common Issues
1. API Connection:
   - Check WiFi connection
   - Verify API credentials
   - Monitor request timing
   - Check SSL verification warnings

2. SPIFFS Operations:
   - Verify initialization
   - Check storage space
   - Monitor file rotation
   - Validate JSON structure

## Running Tests

1. Configure test mode in config.h
2. Upload code to device
3. Monitor Serial output
4. Observe LED feedback
5. Use button to:
   - Start test
   - Retry on error
   - Run new test after completion

## Performance Notes

1. API Requests:
   - Default 2s delay between requests
   - Connection reuse reduces overhead
   - SSL handshake optimization
   - Timing information in debug output

2. Storage Operations:
   - Maximum 5 stored quotes
   - Automatic file rotation
   - JSON format for structured storage

## Adding New Tests

1. Define test mode in config.h
2. Implement test logic in TestManager
3. Add visual feedback states
4. Update documentation

## Maintenance

- Keep test modes mutually exclusive
- Maintain consistent debug output
- Update documentation with changes
- Regular testing of all modes
- Monitor performance metrics
