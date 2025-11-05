#include "wifi_connect.h"

void connectWiFi(const char* ssid, const char* password) {
  Serial.print("🔌 正在连接 WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n WiFi 已连接");
    Serial.print("IP 地址: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi 连接失败，请检查SSID或密码");
  }
}

bool ensureWiFiConnected(const char* ssid, const char* password) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(" WiFi 断开，正在重新连接...");
    connectWiFi(ssid, password);
  }
  return WiFi.status() == WL_CONNECTED;
}
