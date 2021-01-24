//红外遥控esp32代码
#include <Arduino.h>
#include "cxg_IRremote.h"

//红外遥控, 需要支持中断的引脚
#define IRremote_PIN 2
uint16_t receiveData = 0;
CxgIRremote cxgIRremote;

void setup() {
  Serial.begin(115200);

  cxgIRremote.attach(IRremote_PIN);
  //arduino uno使用中断0或1
  //ESP32,stm32等直接使用IRremote_PIN引脚
  attachInterrupt(
    0, []() {
      cxgIRremote.handleRisingInterrupt();
    },
    RISING);
}

void loop() {
  //可以返回只返回16位
  // uint16_t data = cxgIRremote.read();
  uint32_t data = cxgIRremote.read32();
  if(data > 0) {
    receiveData = data;
    Serial.print("receive: ");
    Serial.println(data, HEX);
  }
}
