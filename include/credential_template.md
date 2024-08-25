Create a `credentials.h` file in the `include` directory with the following content:

   ```cpp
   #ifndef CREDENTIALS_H
   #define CREDENTIALS_H

   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   const char* apiKey = "YOUR_CLAUDE_API_KEY";

   #endif
   ```