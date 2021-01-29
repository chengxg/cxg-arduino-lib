#include <Arduino.h>
#include "cxg_JSTime.h"

//支持arduino uno, stm32, esp32
static JSTime jsTime;
int timeId = 0;
int i = 0;
int j = 0;

void setup() {
  Serial.begin(115200);
  delay(3000);

  //每隔1s执行一次, 执行三次取消执行
  timeId = jsTime.setInterval([]() {
    Serial.println("setInterval 1000ms");
    i++;
    if(i == 3) {
      Serial.print("clearTime: ");
      Serial.println(timeId);
      //清除定时器, 如果不指定timeId,则全部取消
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

  jsTime.setTimeout([]() {
    Serial.println("bug: no clear all time");
  },
    7000);

  jsTime.setInterval([]() {
    j++;
    if(j == 5) {
      //清除全部定时器
      bool isClearSuccess = jsTime.clearTime(0);
      if(isClearSuccess) {
        Serial.println("clear all Success");
        jsTime.setTimeout([]() {
          Serial.println("test success");
        },
          1000);
      }
    }
  },
    1000);
}

void loop() {
  jsTime.refresh();
}
