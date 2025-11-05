#include "dht_sensor.h"
#include "config.h"
#include "http_upload.h"
#include <Arduino.h>
#include "LED.h"

static DHT dht(DHT_PIN, DHT_TYPE); // 使用配置文件中定义的引脚

void initDHT(uint8_t pin) {
  dht = DHT(pin, DHT_TYPE);
  dht.begin();
}

SensorData readDHT() {
  SensorData data;
  data.humidity = dht.readHumidity();
  data.temperature = dht.readTemperature();

  if (isnan(data.humidity) || isnan(data.temperature)) {
    data.valid = false;
  } else {
    data.valid = true;
  }

    // 检查读取是否成功
    if (isnan(data.humidity) || isnan(data.temperature)) {
        Serial.println("读取DHT传感器失败");
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
    }

    // 如果数据有效，打印并上传
    if (data.valid) {
        // 打印数据
        Serial.printf("室内温度: %.1f°C, 室内湿度: %.1f%%\n", 
                     data.temperature, data.humidity);

        // 上传数据到服务器
        uploadData(SERVER_URL, data.temperature, data.humidity);

        // 指示数据处理完成
        blinkLED();
    }

    return data;  // 返回传感器数据
}
