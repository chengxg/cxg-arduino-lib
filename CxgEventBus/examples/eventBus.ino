#include <Arduino.h>
#include "cxg_EventBus.h"

//支持arduino uno, stm32, esp32
//仿照javascript的事件总线实现的库
CxgEventBus eventBus;

void setup() {
  Serial.begin(115200);
  //注册事件, 可以多次执行, 第三个参数表示是否覆盖已注册的事件
  eventBus.on(
    "test1", [](void* params) {
      //获取参数
      int a = *( int* )params;
      Serial.println("exeTest1");
      Serial.println(a);
    },
    false);

  //注册事件, 仅执行一次
  eventBus.once(
    "once", [](void* params) {
      Serial.println("exe once");
    },
    false);
}
void loop() {
  while(Serial.available()) {
    char d = Serial.read();
    if(d == 'a') {
      int a = 32;
      //传递一个参数
      eventBus.emit("test1", &a);
    }
    if(d == 'b') {
      eventBus.emit("once", NULL);
    }
  }
}
