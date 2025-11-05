#include "epaper_display.h"
#include <GxEPD2_3C.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "UbuntuMono_R/UbuntuMono_R_18pt8b.h"
#include "UbuntuMono_R/UbuntuMono_R_16pt8b.h"
#include "UbuntuMono_R/UbuntuMono_R_14pt8b.h"
#include "UbuntuMono_R/UbuntuMono_R_12pt8b.h"
#include "UbuntuMono_R/UbuntuMono_R_10pt8b.h"

// ESP32 主控接法：CS=5, DC=4, RST=15, BUSY=19
// 三色屏（黑、白、红）需要用 GxEPD2_3C，它会自动分离黑层和红层
GxEPD2_3C<GxEPD2_370C_UC8253, GxEPD2_370C_UC8253::HEIGHT> display(
  GxEPD2_370C_UC8253(/*CS=*/ 5, /*DC=*/ 4, /*RST=*/ 15, /*BUSY=*/ 19)
);

void initEpaperDisplay() {
  Serial.println("\n========== 电子墨水屏初始化开始 ==========");
  Serial.println("屏幕类型: 三色屏 (黑/白/红)");
  
  Serial.println("引脚配置:");
  Serial.println("  CS   = 5");
  Serial.println("  DC   = 4");
  Serial.println("  RST  = 15");
  Serial.println("  BUSY = 19");
  Serial.println("  MOSI = 23 (硬件SPI)");
  Serial.println("  SCK  = 18 (硬件SPI)");
  
  // 设置引脚为输出模式
  pinMode(5, OUTPUT);   // CS
  pinMode(4, OUTPUT);   // DC
  pinMode(15, OUTPUT);  // RST
  pinMode(19, INPUT);   // BUSY (输入)
  Serial.println("✓ 引脚模式设置完成");
  
  // 墨水屏初始化
  Serial.println("正在初始化三色屏驱动...");
  display.init(115200); // baud rate for SPI
  Serial.println("✓ 墨水屏驱动初始化完成");
  
  // 设置横屏显示（旋转90度）
  display.setRotation(1);  // 0=竖屏, 1=横屏, 2=反向竖屏, 3=反向横屏
  Serial.println("✓ 屏幕方向设置为横屏");
  
  // 设置GFX的字体
  display.setFont(&UbuntuMono_R_16pt8b);
  Serial.println("✓ 字体设置完成");
  
  // 清屏测试
  Serial.println("正在清屏...");
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);  // 白色底色
  } while (display.nextPage());
  Serial.println("✓ 清屏完成");
  
  // 显示测试信息（黑色）
  Serial.println("正在显示初始化完成提示...");
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);  // 白色底色
    display.setFont(&UbuntuMono_R_16pt8b);
    display.setTextColor(GxEPD_BLACK);  // 黑色字体
    display.setCursor(10, 50);
    display.println("Init OK!");
  } while (display.nextPage());
  
  Serial.println("========== 电子墨水屏初始化完成 ==========\n");
}

void clearEpaperDisplay() {
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);  // 白色底色
  } while (display.nextPage());
}

void displayWeatherInfo(const WeatherInfo& weatherInfo, const SensorData& sensorData) {
  if (!weatherInfo.valid) {
    Serial.println("天气数据无效，跳过显示");
    return;
  }

  Serial.println("开始更新墨水屏显示...");
  
  // 全局刷新
  display.setFullWindow();
  display.firstPage();
  do {
    // 白色底色
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);  // 黑色字体
    
    // ========== 标题 ==========
    display.setFont(&UbuntuMono_R_16pt8b);
    display.setCursor(10, 30);
    display.println("WEATHER");
    
    // 分割线（黑色）
    display.drawFastHLine(10, 35, 400, GxEPD_BLACK);
    
    // ========== API 天气数据 ==========
    display.setFont(&UbuntuMono_R_12pt8b);
    
    // 第一行：城市
    display.setCursor(10, 55);
    display.print("Location: ");
    display.println(weatherInfo.location.c_str());
    
    // 第二行：经纬度
    display.setCursor(10, 75);
    display.print("Lat: ");
    display.print(weatherInfo.lat, 3);
    display.print(" Lon: ");
    display.println(weatherInfo.lon, 3);
    
    // 第三行：温度 和 湿度 并排
    display.setCursor(10, 95);
    display.print("Temp: ");
    display.print(weatherInfo.temp_c, 1);
    display.print("C");
    
    display.setCursor(200, 95);
    display.print("Humidity: ");
    display.print((int)weatherInfo.humidity);
    display.print("%");
    
    // 第四行：天气状况
    display.setCursor(10, 115);
    display.print("Condition: ");
    display.println(weatherInfo.condition.c_str());
    
    // 第五行：风向和风速
    display.setCursor(10, 135);
    display.print("Wind: ");
    display.print(weatherInfo.wind_dir.c_str());
    display.print(" ");
    display.print(weatherInfo.wind_kph, 0);
    display.print("km/h");
    
    // 第六行：气压
    display.setCursor(10, 155);
    display.print("Pressure: ");
    display.print((int)weatherInfo.pressure_mb);
    display.print("hPa");
    
    // 分割线（黑色）
    display.drawFastHLine(10, 160, 400, GxEPD_BLACK);
    
    // ========== 本地传感器 ==========
    display.setFont(&UbuntuMono_R_14pt8b);
    display.setCursor(10, 182);
    display.println("LOCAL SENSOR");
    
    display.setFont(&UbuntuMono_R_12pt8b);
    
    if (sensorData.valid) {
      // 温度和湿度并排
      display.setCursor(10, 202);
      display.print("Temp: ");
      display.print(sensorData.temperature, 1);
      display.print("C");
      
      display.setCursor(200, 202);
      display.print("Humidity: ");
      display.print(sensorData.humidity, 1);
      display.print("%");
    } else {
      display.setCursor(10, 202);
      display.println("Sensor Error!");
    }
    
    // 分割线（黑色）
    display.drawFastHLine(10, 207, 400, GxEPD_BLACK);
    
    // ========== 时间戳 ==========
    display.setFont(&UbuntuMono_R_10pt8b);
    display.setCursor(10, 225);
    display.print("Update: ");
    display.println(weatherInfo.localtime.c_str());
    
  } while (display.nextPage());
  
  Serial.println("✓ 天气信息已显示到墨水屏");
}

void displaySensorData(const SensorData& sensorData) {
  Serial.println("开始更新墨水屏显示（传感器数据）...");
  display.setFullWindow();
  display.firstPage();
  do {
    // 白色底色
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);  // 黑色字体
    
    display.setFont(&UbuntuMono_R_16pt8b);
    display.setCursor(10, 20);
    display.println("SENSOR DATA");
    
    display.drawFastHLine(10, 22, 400, GxEPD_BLACK);
    
    display.setFont(&UbuntuMono_R_14pt8b);
    
    if (sensorData.valid) {
      display.setCursor(10, 60);
      display.print("Temperature: ");
      display.print(sensorData.temperature, 1);
      display.println("C");
      
      display.setCursor(10, 85);
      display.print("Humidity: ");
      display.print(sensorData.humidity, 1);
      display.println("%");
    } else {
      display.setCursor(10, 60);
      display.println("Sensor Error!");
      display.setCursor(10, 85);
      display.println("Check DHT connection!");
    }
    
  } while (display.nextPage());
  
  Serial.println("✓ 传感器数据已显示到墨水屏");
}
