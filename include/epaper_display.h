#ifndef EPAPER_DISPLAY_H
#define EPAPER_DISPLAY_H

#include <Arduino.h>
#include <GxEPD2_3C.h>
#include "GxEPD2_370C_UC8253.h"
#include "weather_api.h"
#include "dht_sensor.h"

// 显示对象声明（在 epaper_display.cpp 中定义）
// 三色屏（黑/白/红）使用 GxEPD2_3C，它会自动分离黑层和红层
extern GxEPD2_3C<GxEPD2_370C_UC8253, GxEPD2_370C_UC8253::HEIGHT> display;

// 初始化显示屏
void initEpaperDisplay();

// 显示天气信息
void displayWeatherInfo(const WeatherInfo& weatherInfo, const SensorData& sensorData);

// 显示传感器数据
void displaySensorData(const SensorData& sensorData);

// 清屏
void clearEpaperDisplay();

#endif
