# Socratic Quote Generator

A device that generates quotes using the Claude AI and displays them on a printer.

## Task Structure

The project is divided into the following tasks:

1. **WiFi Management**: Connect to WiFi network, handle disconnections, and maintain a stable connection.
2. **API Call**: Send requests to the Claude AI to generate quotes.
3. **Parsing Result**: Parse the API response to extract the quote.
4. **Printer Management**: Send the quote to the printer for printing.
5. **Input Handling**: Handle button presses and potential sensor inputs.
6. **Feedback State**: Use the Neopixel to display the device's state (e.g., connected, printing, error).

## Architecture

The project will use a simple state machine architecture to enable multitasking. The main states are:

1. **Initialization**: Initialize the device, connect to WiFi, and set up the printer.
2. **Idle**: Wait for button presses or sensor inputs.
3. **Quote Generation**: Send API request, parse response, and send quote to printer.
4. **Printing**: Handle printer output and display feedback state.
5. **Error**: Handle errors and display error state.

## Modules

The project will be divided into the following modules:

1. **WiFi Manager**: Handles WiFi connection and disconnection.
2. **API Client**: Sends requests to the Claude AI and handles responses.
3. **Quote Parser**: Parses API responses to extract quotes.
4. **Printer Driver**: Sends quotes to the printer for printing.
5. **Input Handler**: Handles button presses and sensor inputs.
6. **Feedback Display**: Displays the device's state using the Neopixel.

Each module will follow the input-process-output (IPO) model:

* **Input**: Receive data or events from other modules or external sources.
* **Process**: Perform processing or logic on the input data.
* **Output**: Send output data or events to other modules or external sources.# ESP32 Socratic Quote Generator

## Overview

This project uses an ESP32 board (stampC3U or stampS3) to generate Socratic quotes based on the current time and date. It demonstrates WiFi connectivity, button input, LED control, time retrieval from an online API, and interaction with the Claude AI to generate philosophical quotes.

## Features

- WiFi connectivity with robust connection handling
- Debounced button input detection
- LED toggle and animation using FreeRTOS
- Time and date retrieval from online API
- Quote generation using Claude AI
- Memory-optimized JSON parsing
- Detailed error handling and debugging output

## Hardware Requirements

- ESP32 board (stampC3U or stampS3)
- USB cable for power and programming
- Thermal printer
- Switch selector
- Momentary pushButton

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
   const char* apiEndpoint = "https://api.anthropic.com/v1/messages";
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