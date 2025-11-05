#include "http_upload.h"

bool uploadData(const char* serverUrl, float temperature, float humidity) {
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    // 创建HTTP客户端
    WiFiClient client;
    HTTPClient http;

    // 构建完整的URL（包含API密钥和数据）
    String url = String(serverUrl);
    url += "?api_key=";
    url += THINGSPEAK_API_KEY;
    url += "&field1=";
    url += String(temperature, 2);  // 温度数据发送到第1个字段
    url += "&field2=";
    url += String(humidity, 2);     // 湿度数据发送到第2个字段

    // 开始HTTP连接
    http.begin(client, url);

    // 发送GET请求
    Serial.println("正在上传数据到ThingSpeak...");
    int httpCode = http.GET();

    // 检查响应
    if (httpCode > 0 && httpCode == HTTP_CODE_OK) {
        http.getString();  // 读取但不打印响应
        Serial.println("数据上传成功");
        http.end();
        return true;
    }
    
    Serial.println("数据上传失败");
    
    http.end();
    return false;
}
