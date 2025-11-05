#ifndef CONFIG_H
#define CONFIG_H

// ==========================
// 🖼 墨水屏参数 (416x240 黑白电子纸屏)
// ==========================
#define EPD_WIDTH  416
#define EPD_HEIGHT 240

// SPI 引脚配置
#define PIN_CS    5      // SS/CS 片选
#define PIN_MOSI  23     // SDA/MOSI 主出从入
#define PIN_SCK   18     // SCK 时钟

// 控制引脚
#define PIN_DC    4      // DC 数据/命令
#define PIN_RST   15     // RST 复位
#define PIN_BUSY  19     // BUSY 忙碌标志

// ==========================
// 🌦 WeatherAPI 设置
// ==========================
#define WEATHER_API_KEY     "05a6ed4fd52741c68fd32622250111"
#define WEATHER_LOCATION    "Changsha"   // 你可以改成城市名或经纬度，如 "Beijing" 或 "31.2,121.5"
#define WEATHER_UPDATE_INTERVAL  10000     // 每 10 秒更新一次天气

 // 内置LED引脚
#define LED_PIN         2        

// ==========================
// WiFi 配置
// ==========================
#define WIFI_SSID       "小琛的iPhone"   // WiFi名称
#define WIFI_PASSWORD   "yemlucre"    // WiFi密码

// ==========================
// 云服务器配置
// ==========================
// 默认使用 Postman Echo 公共测试接口
// 可以改为 ThingSpeak 或你自己的服务器
#define THINGSPEAK_API_KEY "7YKKWTPKZ3WSCS2C"    // ThingSpeak Write API Key
#define SERVER_URL          "http://api.thingspeak.com/update"  // ThingSpeak API 端点

// ==========================
// 🌡 DHT11 配置
// ==========================
#define DHT_PIN          13        // DHT11 数据引脚
#define DHT_TYPE         DHT11    // 若换成 DHT22 可直接改这里
// ==========================
// ⏱ 上传周期（毫秒）
// ==========================
#define READING_INTERVAL 300000     // 读取间隔（300秒）
#define WIFI_TIMEOUT    20000     // WiFi连接超时时间
#define LED_BLINK_TIME   100      // LED闪烁时间

// ==========================
// 调试设置
// ==========================
#define SERIAL_BAUDRATE  115200   // 串口波特率

// ==========================
// 系统信息（可选）
// ==========================
// 用于打印或版本追踪
#define PROJECT_NAME     "ESP32_DHT11_HTTP_Uploader"
#define PROJECT_VERSION  "v1.0.0"

#endif
