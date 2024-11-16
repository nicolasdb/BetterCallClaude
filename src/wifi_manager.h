#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

void wifi_manager_init();
void wifi_manager_connect(const char* ssid, const char* password);
void wifi_manager_disconnect();

#endif  // WIFI_MANAGER_H