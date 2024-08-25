# ESP32 Socratic Quote Generator

## Overview

This project uses an ESP32 board (stampC3U or stampS3) to generate Socratic quotes based on the current time and date. It demonstrates WiFi connectivity, button input, LED control, time retrieval from an online API, and interaction with the Claude AI to generate philosophical quotes.

## Features

- WiFi connectivity with robust connection handling
- Debounced button input detection
- LED toggle and animation using FreeRTOS
- Time and date retrieval from online API
- Socratic quote generation using Claude AI
- Memory-optimized JSON parsing
- Detailed error handling and debugging output

## Hardware Requirements

- ESP32 board (stampC3U or stampS3)
- USB cable for power and programming

## Software Requirements

- PlatformIO IDE (or Arduino IDE with necessary ESP32 board support)
- Required libraries: WiFi, HTTPClient, ArduinoJson, FastLED

## Setup Instructions

1. Clone this repository to your local machine.
2. Open the project in PlatformIO IDE.
3. Create a `credentials.h` file in the `include` directory with the following content:

   ```cpp
   #ifndef CREDENTIALS_H
   #define CREDENTIALS_H

   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   const char* apiKey = "YOUR_CLAUDE_API_KEY";

   #endif
   ```

4. Replace `YOUR_WIFI_SSID`, `YOUR_WIFI_PASSWORD`, and `YOUR_CLAUDE_API_KEY` with your actual WiFi credentials and Claude API key.
5. Select the appropriate board environment in `platformio.ini`:
   - For stampC3U: `default_envs = stampC3U`
   - For stampS3: `default_envs = stampS3`

## Usage

1. Upload the code to your ESP32 board.
2. Open the Serial Monitor (baud rate: 115200).
3. The board will attempt to connect to WiFi. If unsuccessful after 20 attempts, it will restart.
4. Once connected, the Serial Monitor will display "Setup complete. Press the button to start."
5. Press the on-board button:
   - For stampC3U: The button is on pin 9
   - For stampS3: The button is on pin 0
6. The board will retrieve the current time, generate a Socratic quote, and display it in the Serial Monitor.
7. The on-board LED will toggle with each quote generation:
   - For stampC3U: The LED is on pin 2
   - For stampS3: The LED is on pin 21
8. LED animation will run continuously in the background using FreeRTOS.

## Debugging

- Set `DEBUG_MODE` to 1 in `config.h` for detailed debug output.
- The code includes memory usage reporting for JSON operations to help optimize document sizes.

## Troubleshooting

If you encounter any issues:

1. Check your WiFi credentials in `credentials.h`.
2. Verify your Claude API key is correct and active.
3. Review the Serial Monitor output for detailed error messages.
4. Ensure you're using the latest version of the ESP32 board support and required libraries.
5. If WiFi connection fails repeatedly, the device will automatically restart.

## Contributing

Feel free to submit issues or pull requests if you have suggestions for improvements or encounter any problems.

## License

This project is open-source and available under the MIT License.