#include "weather_api.h"
#include "config.h"

WeatherInfo getWeather(const char* apiKey, const char* location) {
    WeatherInfo info;
    info.valid = false;
    const int MAX_RETRIES = 3;  // 最大重试次数
    const int RETRY_DELAY = 2000;  // 重试间隔(毫秒)

    // 检查WiFi连接
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi未连接，无法获取天气数据");
        return info;
    }

    // 创建HTTP客户端
    HTTPClient http;
    WiFiClient client;

    // 构建请求URL
    String url = "http://api.weatherapi.com/v1/current.json?key=" + String(apiKey) +
                "&q=" + String(location) + "&aqi=no";

    // 配置HTTP请求
    http.begin(client, url);
    http.setTimeout(15000);  // 增加超时时间到15秒
    http.addHeader("User-Agent", "ESP32WeatherStation/1.0");
    http.addHeader("Accept", "application/json");
    http.setReuse(true);  // 启用连接重用

    // 发送GET请求，带重试机制
    int httpCode = 0;
    String payload;
    bool requestSuccess = false;
    
    for (int retry = 0; retry < MAX_RETRIES && !requestSuccess; retry++) {
        if (retry > 0) {
            Serial.printf("第%d次重试获取天气数据...\n", retry);
            delay(RETRY_DELAY);
        }

        httpCode = http.GET();
        
        if (httpCode > 0) {
            if (httpCode == HTTP_CODE_OK || httpCode == 200) {
                payload = http.getString();
                requestSuccess = true;
            } else {
                Serial.printf("HTTP错误: %d\n", httpCode);
            }
        } else {
            Serial.printf("连接失败: %s\n", http.errorToString(httpCode).c_str());
        }
    }

    if (requestSuccess) {
        // 解析JSON数据
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, payload);
            
            if (!error) {
                // 提取核心天气信息
                info.location = doc["location"]["name"].as<String>();
                info.region = doc["location"]["region"].as<String>();
                info.country = doc["location"]["country"].as<String>();
                info.lat = doc["location"]["lat"].as<float>();
                info.lon = doc["location"]["lon"].as<float>();
                info.localtime = doc["location"]["localtime"].as<String>();
                info.condition = doc["current"]["condition"]["text"].as<String>();
                info.temp_c = doc["current"]["temp_c"].as<float>();
                info.humidity = doc["current"]["humidity"].as<float>();
                info.wind_kph = doc["current"]["wind_kph"].as<float>();
                info.wind_dir = doc["current"]["wind_dir"].as<String>();
                info.feelslike_c = doc["current"]["feelslike_c"].as<float>();
                info.pressure_mb = doc["current"]["pressure_mb"].as<float>();
                info.vis_km = doc["current"]["vis_km"].as<float>();
                info.uv = doc["current"]["uv"].as<float>();
                info.gust_kph = doc["current"]["gust_kph"].as<float>();
                info.precip_mm = doc["current"]["precip_mm"].as<float>();

                info.valid = true;

                // 打印精简的天气信息
                Serial.println("\n=== 天气实况 ===");
                Serial.printf("位置: %s, %s, %s\n", 
                    info.location.c_str(),
                    info.region.c_str(),
                    info.country.c_str());
                Serial.printf("经纬度: %.4f°N  %.4f°E \n", 
                    info.lat, 
                    info.lon);
                Serial.printf("时间: %s\n", info.localtime.c_str());
                Serial.printf("温度: %.1f°C", info.temp_c);
                if (info.feelslike_c != info.temp_c) {
                    Serial.printf(" (体感 %.1f°C)", info.feelslike_c);
                }
                Serial.println();
                Serial.printf("湿度: %.1f%%\n", info.humidity);
                Serial.printf("风向: %s %.1f km/h\n", info.wind_dir.c_str(), info.wind_kph);
                Serial.printf("天气: %s\n", info.condition.c_str());
                Serial.printf("气压: %.0f hPa\n", info.pressure_mb);
            } else {
                Serial.printf("JSON解析失败: %s\n", error.c_str());
            }
    } else {
        if (httpCode > 0) {
            Serial.printf("天气API返回错误: %d\n", httpCode);
            String errorResponse = http.getString();
            if (errorResponse.length() > 0) {
                Serial.println("错误信息: " + errorResponse);
            }
        } else {
            Serial.println("无法连接到天气服务器");
        }
    }

    // 清理HTTP资源并关闭连接
    http.end();
    client.stop();
    
    return info;
}