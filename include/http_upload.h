#ifndef HTTP_UPLOAD_H
#define HTTP_UPLOAD_H

#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"

// 上传数据到ThingSpeak服务器
// 返回值：true表示上传成功，false表示上传失败
bool uploadData(const char* serverUrl, float temperature, float humidity);

#endif
