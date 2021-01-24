#include <Arduino.h>
#include "cxg_EventBus.h"

//支持arduino uno, stm32, esp32
CxgEventBus eventBus;

void setup() {
  Serial.begin(115200);
  eventBus.on(
    "test1", [](void* params) {
      int a = *( int* )params;
      Serial.println("exeTest1");
      Serial.println(a);
    },
    false);

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
      eventBus.emit("test1", &a);
    }
    if(d == 'b') {
      eventBus.emit("once", NULL);
    }
  }
}
