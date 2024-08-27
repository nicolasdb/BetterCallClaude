Create a `credentials.h` file in the `include` directory with the following content:

   ```cpp
   #ifndef CREDENTIALS_H
   #define CREDENTIALS_H

   // WiFi credentials
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   // Claude API configuration
   const char* apiEndpoint = "https://api.anthropic.com/v1/messages";
   const char* apiKey = "YOUR_CLAUDE_API_KEY";

   #endif
   ```