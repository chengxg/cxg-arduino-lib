#include <Arduino.h>
#include "cxg_JSTime.h"

//支持arduino uno, stm32, esp32
static JSTime jsTime;

void test(int p1, void* p2) {
  Serial.print("Current exe: ");
  Serial.println(p1);
}

void setup() {
  Serial.begin(115200);
  delay(3000);

  //初始化20个定时器看看
  for(int i = 1; i <= 20; i++) {
    jsTime.setTimeout(test, 1000 * i, i);
  }
  int heapSize = jsTime.getHeapMemorySize();
  Serial.print("setTimeout heapSize: ");
  Serial.println(heapSize);
}

void loop() {
  jsTime.refresh();
}
