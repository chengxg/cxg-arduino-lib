#include <Arduino.h>
#include "cxg_JSTime.h"

int size = 30;

//支持arduino uno, stm32, esp32
static JSTime jsTime;

void test(int p1, void* p2) {
  Serial.print("Current index: ");
  Serial.println(p1);
  if(p1 == size) {
    Serial.print("exe end, num: ");
    Serial.println(jsTime.getNumber());
    int heapSize = jsTime.getHeapMemorySize();
    Serial.print("end heapSize: ");
    Serial.println(heapSize);
  }
}

void setup() {
  Serial.begin(115200);
  delay(3000);

  //初始化很多个定时器看看
  int clearTimeIds[size] = {0};
  for(int i = 1; i <= size; i++) {
    clearTimeIds[i - 1] = jsTime.setTimeout(test, 1000 * i, i);
  }
  //取消单个定时器
  jsTime.clearTime(clearTimeIds[2]);
  //同时取消3个定时器
  jsTime.clearTime(clearTimeIds, 5, 3);

  int heapSize = jsTime.getHeapMemorySize();
  Serial.print("JSTimeStruct Size: ");
  Serial.println(sizeof(JSTimeStruct));  //32
  Serial.print("all heapSize: ");
  Serial.println(heapSize);  //764
}

void loop() {
  jsTime.refresh();
}
