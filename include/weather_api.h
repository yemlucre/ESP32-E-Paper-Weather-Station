#ifndef WEATHER_API_H
#define WEATHER_API_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

struct WeatherInfo {
  bool valid;
  String location;      // 城市名
  String region;        // 地区/省份
  String country;       // 国家
  float lat;           // 纬度
  float lon;           // 经度
  String condition;     // 天气状况文字
  float temp_c;         // 温度（摄氏）
  float humidity;       // 湿度（百分比）
  float wind_kph;       // 风速
  String wind_dir;      // 风向
  float feelslike_c;    // 体感温度
  float pressure_mb;    // 气压（百帕）
  float vis_km;        // 能见度（公里）
  float uv;            // 紫外线指数
  float gust_kph;      // 阵风速度
  float precip_mm;     // 降水量（毫米）
  String localtime;     // 当地时间
};

WeatherInfo getWeather(const char* apiKey, const char* location);

#endif
