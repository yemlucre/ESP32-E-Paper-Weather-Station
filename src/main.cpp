#include <Arduino.h>
#include "config.h"
#include "dht_sensor.h"
#include "wifi_connect.h"
#include "http_upload.h"
#include "LED.h"
#include "weather_api.h"
#include "epaper_display.h"

unsigned long lastWeatherUpdate = 0;
unsigned long lastUpload = 0;

void setup() {
    // 初始化串口
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println("\n=== ESP32天气站 ===\n");

    // 初始化LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // 初始化DHT传感器
    initDHT(DHT_PIN);
    Serial.println("DHT传感器初始化完成");

    // 初始化电子墨水屏
    initEpaperDisplay();

    // 连接WiFi
    connectWiFi(WIFI_SSID, WIFI_PASSWORD);

    // 一旦 WiFi 连接上，立即尝试获取天气并刷新显示（如果失败则重复重试直到成功）
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi 已连接，首次刷新天气数据...");
        WeatherInfo info;
        // 持续重试，直到成功获取天气数据
        while (true) {
            info = getWeather(WEATHER_API_KEY, WEATHER_LOCATION);
            if (info.valid) {
                Serial.println("首次天气数据获取成功");
                break;
            }
            Serial.println("首次天气数据获取失败，5秒后重试...");
            // 如果 WiFi 断开则尝试重连
            if (WiFi.status() != WL_CONNECTED) {
                Serial.println("WiFi 断开，尝试重新连接...");
                connectWiFi(WIFI_SSID, WIFI_PASSWORD);
            }
            delay(5000);
        }
        // 获取本地传感器并显示（在成功获取天气后再显示以保持信息一致）
        SensorData sensorData = readDHT();
        lastWeatherUpdate = millis();
        displayWeatherInfo(info, sensorData);
    } else {
        Serial.println("WiFi 未连接，跳过首次天气刷新");
    }
}

void loop() {
    // 确保WiFi连接
    if (!ensureWiFiConnected(WIFI_SSID, WIFI_PASSWORD)) {
        delay(5000);  // 如果WiFi未连接，等待5秒后重试
        return;
    }

    // 处理天气更新（按 WEATHER_UPDATE_INTERVAL 设置触发，已在 config.h 中设为 10s）
    if (millis() - lastWeatherUpdate >= WEATHER_UPDATE_INTERVAL) {
        Serial.println("\n========== 开始获取天气数据 ==========");
        lastWeatherUpdate = millis();

        // 如果 WiFi 连着则请求天气 API，否则仅显示本地传感器数据
        if (WiFi.status() == WL_CONNECTED) {
            WeatherInfo info = getWeather(WEATHER_API_KEY, WEATHER_LOCATION);
            SensorData sensorData = readDHT();
            if (info.valid) {
                Serial.println("✓ 天气数据获取成功，更新显示");
                displayWeatherInfo(info, sensorData);
            } else {
                Serial.println("✗ 天气数据获取失败（API 响应无效），显示本地传感器数据");
                displaySensorData(sensorData);
            }
        } else {
            Serial.println("WiFi 未连接，显示本地温湿度");
            SensorData sensorData = readDHT();
            displaySensorData(sensorData);
        }

        Serial.println("========== 天气数据处理完成 ==========");
    }

    // 等待（避免频繁的loop循环）
    delay(1000);
}
