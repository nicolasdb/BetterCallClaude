# SPIFFS Quote Storage Module Testing Guide

## Overview
This test suite validates the functionality of the SPIFFSManager module for quote storage in the Quote Generator project.

## Test Scenarios

### 1. SPIFFS Initialization
- Verify successful SPIFFS mounting
- Confirm quote storage directory creation

### 2. Quote Saving Mechanism
- Test saving individual quotes
- Validate quote storage with JSON formatting
- Verify timestamp inclusion

### 3. Quote Rotation
- Save more quotes than `QUOTES_MAX_STORED`
- Confirm only the latest quotes are retained
- Validate automatic deletion of oldest quotes

### 4. Quote Retrieval
- Load the most recently stored quote
- Verify quote content accuracy
- Check JSON parsing functionality

### 5. Quote Listing
- List all stored quotes
- Confirm correct file naming
- Validate quote file tracking

## Test Setup

### Hardware Requirements
- ESP32 board (stampC3U or stampS3)
- USB cable for programming and power

### Software Requirements
- PlatformIO IDE
- Arduino framework
- ArduinoJson library
- SPIFFS library

## Running the Test

1. Open the project in PlatformIO
2. Upload `SPIFFSManager_test.cpp`
3. Open Serial Monitor (115200 baud)
4. Observe test output and results

## Expected Outputs

### Successful Test
- SPIFFS initialization message
- Quotes saved successfully
- Quotes listed correctly
- Last quote retrieved

### Potential Error Scenarios
- SPIFFS mount failure
- Insufficient storage space
- JSON parsing errors

## Debugging Tips
- Ensure `config.h` has correct SPIFFS settings
- Check available SPIFFS storage
- Verify ArduinoJson library compatibility

## Troubleshooting
- If tests fail, review Serial Monitor for specific error messages
- Verify board-specific SPIFFS configuration
- Check for file system corruption

## Notes
- This test provides basic functionality validation
- Additional edge case testing may be required
