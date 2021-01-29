#include <Arduino.h>
#include "cxg_JSTime.h"

//支持arduino uno, stm32, esp32
static JSTime jsTime;
int timeId = 0;

//跑上一天看看有没有问题
void exeTimeout() {
  timeId = jsTime.setTimeout(exeTimeout, 2);
  Serial.print("next timeId: ");
  Serial.println(timeId);
}

void setup() {
  Serial.begin(115200);
  delay(4000);

  timeId = jsTime.setTimeout(exeTimeout, 1000);
}

void loop() {
  jsTime.refresh();
}
