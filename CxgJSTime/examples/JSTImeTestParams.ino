#include <Arduino.h>
#include "cxg_JSTime.h"

//支持arduino uno, stm32, esp32
static JSTime jsTime;

struct TestStruct {
  int a = 0;
  int b = 0;
};

//测试参数传递
void testTimeoutParams(int p1, void* p2) {
  Serial.println("params test");
  Serial.print("get param1: ");
  Serial.println(p1);
  struct TestStruct* param = ( struct TestStruct* )p2;
  if(param != NULL) {
    Serial.print("get param2 struct p->a:");
    Serial.println(param->a);
    Serial.print("get param2 struct p->b:");
    Serial.println(param->b);

    free(param);
    param = NULL;
  }
}

void setup() {
  Serial.begin(115200);
  jsTime.setTimeout([]() {
    //动态创建一个结构体
    struct TestStruct* param = ( struct TestStruct* )malloc(sizeof(struct TestStruct));
    if(param == NULL) {
      Serial.println("fail create TestStruct");
      return;
    }
    param->a = 123;
    param->b = 456;

    jsTime.setTimeout(testTimeoutParams, 2000, 789, param);
  },
    3000);
}

void loop() {
  jsTime.refresh();
}
