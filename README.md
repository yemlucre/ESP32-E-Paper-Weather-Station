ESP32 天气站 — 项目详尽说明
=====================================

本文件为项目的中文说明（适合课程论文或项目回顾），包含硬件接线、模块职责、运行流程、编译环境、调试要点与扩展建议。

目录
----

- 项目概述
- 硬件清单与接线
- 软件总体架构
  - 主要模块与职责
  - 数据结构说明
  - 程序执行流程（时序）
- 电子墨水屏（3色）处理细节
- 字体与排版说明
- 构建与上传（PlatformIO / Windows）
- 运行日志与验证步骤
- 常见问题与排查方法
- 测试建议
- 局限与改进建议
- 附录：文件清单与关键代码片段

项目概述
--------

该项目基于 ESP32（Arduino 框架），实现一个小型的天气站：

- 连接 WiFi 并从 WeatherAPI 获取当前天气（JSON）
- 读取本地 DHT 温湿度传感器
- 将传感器数据上传到云（默认 ThingSpeak）
- 在三色电子墨水屏（黑/白/红）上显示天气和本地传感器数据
- 使用内置位图字体以保证墨水屏上文字清晰可读

硬件清单与接线
----------------

推荐硬件：

- ESP32 开发板（`esp32dev`）
- 三色电子墨水屏（控制器：UC8253，分辨率本项目使用 240×416 横向显示）
- DHT11 或 DHT22 温湿度传感器
- 一颗状态 LED（指示网络或上传状态，可选）

默认代码中的接线（参见 `include/config.h`）：

- SPI（硬件）
  - MOSI (SDA) -> GPIO 23
  - SCK -> GPIO 18
- 电子墨水屏控制引脚
  - CS  -> GPIO 5
  - DC  -> GPIO 4
  - RST -> GPIO 15
  - BUSY -> GPIO 19  （输入，用于检测屏幕忙/空闲）
- DHT 数据脚 -> GPIO 13
- LED -> GPIO 2

软件总体架构
-------------

项目文件组织（高层）：

- `src/`：源码实现
  - `main.cpp`：程序入口与主循环
  - `epaper_display.cpp`：所有显示初始化与绘制逻辑（使用 GxEPD2_3C 三色驱动）
  - `weather_api.cpp`：请求 WeatherAPI 并解析 JSON
  - `dht_sensor.cpp`：DHT 初始化与读取
  - `wifi_connect.cpp`：WiFi 连接与重连逻辑
  - `http_upload.cpp`：上报数据到服务器（ThingSpeak）
- `include/`：头文件与字体资源
  - `config.h`：所有常量配置（引脚、WiFi、API Key、间隔）
  - `weather_api.h`、`dht_sensor.h`、`epaper_display.h` 等
  - 嵌入字体目录：`UbuntuMono_R/`、`Quicksand_Regular/`
- `platformio.ini`：PlatformIO 构建配置与依赖
- `README_zh.md`：本中文说明（项目根）

主要模块与职责
----------------

1. `main.cpp`
   - 初始化：启动串口、调用 `initDHT()`、`initEpaperDisplay()`、连接 WiFi。
   - 主循环：每隔 `WEATHER_UPDATE_INTERVAL`（默认 60 秒）触发一次天气获取；获取成功则读取本地传感器并在屏幕上显示；失败则仅显示本地传感器数据。

2. `wifi_connect.cpp` / `wifi_connect.h`
   - `connectWiFi(ssid, password)`：阻塞式尝试连接（有重试限制），连接成功打印 IP 地址。
   - `ensureWiFiConnected()`：检查并在断线时重连。

3. `weather_api.cpp` / `weather_api.h`
   - `getWeather(apiKey, location)`：使用 `HTTPClient` 向 WeatherAPI 的 `current.json` 接口发起 GET 请求。解析返回的 JSON（使用 `ArduinoJson`），填充 `WeatherInfo` 结构体并返回。
   - 解析字段包括：位置、经纬度、localtime、温度、湿度、风速/方向、气压、能见度、紫外线、降水、阵风等。

4. `dht_sensor.cpp` / `dht_sensor.h`
   - `initDHT(pin)`：DHT 初始化。
   - `readDHT()`：读取温湿度；成功时会调用 `uploadData()` 将数据上报至 `SERVER_URL`（ThingSpeak），并通过 LED 闪烁提示。

5. `epaper_display.cpp` / `epaper_display.h`
   - 使用 `GxEPD2_3C<GxEPD2_370C_UC8253, HEIGHT>`，这是三色面板（黑/白/红）的正确驱动类。
   - 提供：`initEpaperDisplay()`、`displayWeatherInfo(weather, sensor)`、`displaySensorData(sensor)`、`clearEpaperDisplay()`。
   - 采用 page-based 渲染：
     - `display.setFullWindow(); display.firstPage(); do { ... } while(display.nextPage());`
   - 使用库定义的颜色常量 `GxEPD_BLACK`, `GxEPD_RED`, `GxEPD_WHITE` 来保证黑红层正确分离。

数据结构说明
-----------

- `WeatherInfo`（见 `include/weather_api.h`）字段：
  - `bool valid`：数据是否有效
  - `String location, region, country`
  - `float lat, lon`
  - `String condition`
  - `float temp_c, humidity, wind_kph, feelslike_c, pressure_mb, vis_km, uv, gust_kph, precip_mm`
  - `String localtime`

- `SensorData`（见 `include/dht_sensor.h`）字段：
  - `bool valid`
  - `float temperature`
  - `float humidity`

程序执行流程（时序）
------------------

1. 启动（setup）
   - 初始化串口（115200）
   - 初始化 DHT、墨水屏、连接 WiFi
2. 主循环（loop）
   - 确保 WiFi 连接
   - 若距上次天气获取超过 `WEATHER_UPDATE_INTERVAL`：
     - 调用 `getWeather()` 获取天气
     - 调用 `readDHT()` 获取本地数据
     - 若天气有效 -> `displayWeatherInfo(weather, sensor)`（在屏幕上显示天气 + 本地传感器）
     - 若天气无效 -> `displaySensorData(sensor)`（仅显示本地传感器）
   - 休眠短时间（1s）以避免空循环

电子墨水屏（3色）处理细节
-----------------------

三色屏（黑/白/红）要点：

- 必须使用 `GxEPD2_3C`（3C 表示 3-color）而非 `GxEPD2_BW`，否则红层不会被正确驱动。
- 渲染采用 page-based 方式，以节省内存。
- 更新时建议使用 `setFullWindow()` 进行全屏刷新以保证显示的一致性，尤其是三色屏在分配黑/红层时更可靠。
- 颜色常量：使用 `GxEPD_BLACK` / `GxEPD_RED` / `GxEPD_WHITE`。避免使用 `0x0000` 等 RGB 颜色值，因为这些不一定会被驱动库映射成正确的层。
- 如果观察到刷新时显示清晰但刷新完成后模糊：通常需要确保 BUSY 引脚接线正确，或强制全刷新（库的不同模式）。

字体与排版说明
----------------

- 项目内嵌了位图字体（`include/UbuntuMono_R/`、`include/Quicksand_Regular/`），使用 Adafruit GFX 的 `setFont()` 接口直接设定。
- 当前显示使用等宽字体 `UbuntuMono_R`，多个字号（10/12/14/16/18pt）供选择。
- 为了在电子墨水屏上获得可读性，字体尺寸与行间距被调整为较大并留出空白边距。

构建与上传（PlatformIO / Windows）
---------------------------------

项目使用 PlatformIO（`platformio.ini`）管理：

关键依赖（见 `platformio.ini` 的 `lib_deps`）：
- adafruit/DHT sensor library
- adafruit/Adafruit Unified Sensor
- bblanchon/ArduinoJson
- HTTPClient
- adafruit/Adafruit GFX Library
- olikraus/U8g2_for_Adafruit_GFX (历史保留，可选)
- ZinggJM/GxEPD2 (e-paper 驱动)

在 Windows PowerShell 中的典型命令（如果 PlatformIO 安装在你的用户目录）：

```powershell
# 编译
C:\Users\<YOUR_USER>\.platformio\penv\Scripts\platformio.exe run

# 上传到 esp32dev 环境
C:\Users\<YOUR_USER>\.platformio\penv\Scripts\platformio.exe run --target upload --environment esp32dev

# 清理构建文件
C:\Users\<YOUR_USER>\.platformio\penv\Scripts\platformio.exe run --target clean --environment esp32dev
```

如果你已将 `pio` 添加到 PATH，则可直接用 `pio run` / `pio run --target upload`。

运行日志与验证步骤
------------------

- 使用串口监视（115200）观察启动与运行日志：
  - "ESP32天气站"（启动）
  - 墨水屏初始化信息
  - WiFi 连接与 IP 地址
  - 获取天气时的 HTTP 状态码与解析结果
  - 渲染完成提示

验证建议：
- 上电后观察串口日志确保 WiFi 连接成功
- 人为触发一次 `displayWeatherInfo()`（或在代码中暂时调用）以验证渲染
- 更改 `WEATHER_UPDATE_INTERVAL` 为较短值（例如 10000 ms）以便快速检验刷新

常见问题与排查方法
-----------------

Common problems and how to resolve them:

1. Display shows red where you expect black
   - Ensure you use `GxEPD2_3C` for three-color displays. Using `GxEPD2_BW` will drop red layer handling.
   - Use library color constants (`GxEPD_BLACK`, `GxEPD_RED`, `GxEPD_WHITE`) rather than raw hex values.

2. Display looks blurry after refresh
   - Some tri-color modules require a full refresh. We use `setFullWindow()` + full page rendering.
   - Ensure BUSY pin is correct and properly wired; the driver waits on BUSY to complete internal updates.

3. Fonts missing or compile errors related to fonts
   - Ensure the header files for embedded fonts are present in `include/UbuntuMono_R/`.
   - If compilation reports missing symbol for a font (e.g., `UbuntuMono_R_16pt8b`) check the header's name and the `setFont()` call.

4. WiFi connection fails
   - Check SSID/password in `include/config.h`.
   - Make sure the router allows the ESP32 (some guest networks block devices).

5. Weather API returns errors or rate-limited
   - Verify `WEATHER_API_KEY` in `config.h` and that WeatherAPI account has sufficient quota.
   - API timeouts can be increased in `src/weather_api.cpp`.

Tests and validation
--------------------

- Unit tests are not included; testing is done by hardware verification.
- Suggested validations to perform:
  - Serial logs during startup should report successful WiFi connection and display init.
  - Trigger `displayWeatherInfo()` with a test `WeatherInfo` object on startup to verify rendering.
  - Temporarily set `WEATHER_UPDATE_INTERVAL` to a small value (e.g. 10000 ms) to test frequent updates.

Limitations and future work
---------------------------

- Power usage: Frequent full-screen refreshes are expensive. For battery operation, consider partial updates or increasing the update interval.
- Error handling: If WeatherAPI fails repeatedly, consider exponential backoff.
- Modularization: `display` code currently mixes layout and drawing; consider a layout manager or template system.
- Tests: add unit tests for parsing (ArduinoJson) and for the display layout generator using host-based mocks.
- Add OTA updates for remote firmware upgrades.

Appendix: file map and quick references
--------------------------------------

Key files and purpose:

- `platformio.ini` — Build configuration and library dependencies.
- `include/config.h` — Pins and config constants (WiFi, API Key, intervals).
- `include/weather_api.h` + `src/weather_api.cpp` — Weather API client and `WeatherInfo` struct.
- `include/dht_sensor.h` + `src/dht_sensor.cpp` — DHT sensor reading and upload hook.
- `include/epaper_display.h` + `src/epaper_display.cpp` — Display init and rendering (uses `GxEPD2_3C`).
- `include/UbuntuMono_R/` — Embedded bitmap fonts used by GFX.

Caveats
-------

- This README is derived from the current project source in this workspace. If you change pin assignments or the display's controller type, update `epaper_display` and `config.h` accordingly.

Contact & citation
------------------

If you use this code in academic work, cite the project and include the modules you modified. Keep the `platformio.ini` library attributions intact when reproducing the setup.

---

If you'd like, I can also:
- Generate a PDF-ready LaTeX summary of this README (suitable for a methods section in a paper).
- Produce a 1–2 page condensed version for the paper's appendix.
- Add inline code comments or Javadocs-style documentation to each function.
