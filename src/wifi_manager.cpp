#include "wifi_manager.h"
#include "credentials.h"

void wifi_manager_init() {
  // Initialize WiFi module
  WiFi.mode(WIFI_STA);
}

void wifi_manager_connect(const char* ssid, const char* password) {
  // Connect to WiFi network
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void wifi_manager_connect() {
  // Connect using stored credentials
  wifi_manager_connect(ssid, password);
}

void wifi_manager_disconnect() {
  // Disconnect from WiFi network
  WiFi.disconnect();
  Serial.println("Disconnected from WiFi");
}

const char* get_api_endpoint() {
  return apiEndpoint;
}

const char* get_api_key() {
  return apiKey;
}
