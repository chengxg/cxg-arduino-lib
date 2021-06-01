#include <Arduino.h>
#include "cxg_Btn.h"

static int btn1_pin = 39;
static CxgBtn btn1;

void setup() {
  Serial.begin(115200);

  //按钮按下回调
  btn1.setBtnKeydown([]() {
    Serial.println("keyDown");
  });
  //按钮抬起回调
  btn1.setBtnKeyUp([]() {
    Serial.println("keyUp");
  });
  //按钮长按回调
  btn1.setBtnLongPress([]() {
    Serial.println("longPress");
  });
  //双击回调
  btn1.setBtnDoubleClickCallback([]() {
    Serial.println("doubleClick");
  });
}

void loop() {
  //循环检查状态
  //如果是一个虚拟按钮 不用attach绑定一个实际引脚
  //可以传入一个回调来获取按钮状态, 不一定是实际的按钮,可以是一个bool变量
  btn1.check([]() -> boolean {
    return digitalRead(btn1_pin);
  });
}
