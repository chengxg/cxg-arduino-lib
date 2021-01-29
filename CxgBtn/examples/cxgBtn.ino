#include <Arduino.h>
#include "cxg_Btn.h"

static int btn1_pin = 39;
static CxgBtn btn1;

void setup() {
  Serial.begin(115200);

  //由构造函数初始化变为, 调用attch方法初始化
  btn1.attach(btn1_pin, false);

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
  btn1.check();
}
