#include "config.h"
#include <Arduino.h>




// LED闪烁函数
void blinkLED() {
    digitalWrite(LED_PIN, HIGH);
    delay(LED_BLINK_TIME);
    digitalWrite(LED_PIN, LOW);
}
