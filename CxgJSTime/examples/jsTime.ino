#include <Arduino.h>
#include "cxg_JSTime.h"

//支持arduino uno, stm32, esp32
static JSTime jsTime;
int timeId = 0;
int i = 0;

void setup() {
  Serial.begin(115200);

  //每隔1s执行一次, 执行三次取消执行
  timeId = jsTime.setInterval([]() {
    Serial.println("setInterval 1000ms");
    i++;
    if(i == 3) {
      //清除延时执行, clearTime 如果不指定timeId,则全部取消
      jsTime.clearTime(timeId);
    }
  },
    1000);

  //无阻塞延时2s
  jsTime.setTimeout([]() {
    Serial.println("setTimeout delay 2000ms");
    //延时嵌套
    jsTime.setTimeout([]() {
      Serial.println("nesting setTimeout delay 1000ms");
    },
      1000);
  },
    2000);
}

void loop() {
  jsTime.refresh();
}
