#include <Arduino.h>
#include "cxg_JSTime.h"

//支持arduino uno, stm32, esp32
static JSTime jsTime;
int timeId = 0;
int lastTimeId = 0;

//跑上一天看看有没有问题
void exeTimeout() {
  timeId = jsTime.setTimeout(exeTimeout, 0.05);
}

void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println("start: ");
  jsTime.setInterval([]() {
    Serial.print("max timeId: ");
    Serial.print(timeId);
    Serial.print(" count: ");
    Serial.println(timeId - lastTimeId);
    lastTimeId = timeId;
  },
    1000);
  timeId = jsTime.setTimeout(exeTimeout, 1000);
}

void loop() {
  jsTime.refresh();
}
