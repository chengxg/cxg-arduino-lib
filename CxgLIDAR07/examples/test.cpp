#include <Arduino.h>
#include "cxg_LIDAR07.h"

//在esp32上测试
//连接设置引脚
#define rx_pin 36
#define tx_pin 33
#define sda_pin 22
#define scl_pin 23

#define USE_UART true
#define USE_IIC false

static CxgLIDAR07 LIDAR07;

void setup() {
  Serial.begin(115200);

  if(USE_UART) {
    Serial1.begin(115200);
    LIDAR07.attachUart(&Serial1);
  }

  if(USE_IIC) {
    Wire1.begin(sda_pin, scl_pin, 400000);
    LIDAR07.attachWire(&Wire1, rx_pin, 0x70);
  }

  LIDAR07.setMeasureDistanceCallback([](uint16_t distance, uint16_t temperature, uint16_t amplitude, uint16_t backgroundLight) {
    if(distance > 0) {
      Serial.print("distance: ");
      Serial.print(distance);
      Serial.print(" temperature: ");
      Serial.print(temperature);
      Serial.print(" amplitude: ");
      Serial.print(amplitude);
      Serial.print(" background: ");
      Serial.print(backgroundLight);
      Serial.println();
    }
  });
}

void loop() {
  LIDAR07.refresh();
  while(Serial.available()) {
    char c = Serial.read();
    //开始测量
    if(c == 's') {
      LIDAR07.startMeasure();
    }
    //停止测量指令, 有bug, 会导致模块死掉
    if(c == 'e') {
      // LIDAR07.stopMeasure(); //发送这个指令有死机的bug
      //先设置为单次测量,再发送开始单次测量指令才能停止
      LIDAR07.setMeasureMode(false, [](bool isMulti) {
        LIDAR07.startMeasure();
      });
    }

    //获取版本
    if(c == 'v') {
      LIDAR07.getVersion([](char* version) {
        Serial.print("version: ");
        Serial.println(version);
      });
    }
    //设置时间间隔
    if(c == 'I') {
      LIDAR07.setMeasureInterval(10, [](uint16_t measureInterval) {
        Serial.print("set interval: ");
        Serial.println(measureInterval);
      });
    }
    //设置 主动被动测量
    if(c == 'M') {
      LIDAR07.setMeasureMode(!LIDAR07.isMeasureMulti, [](bool isMulti) {
        Serial.print("set mode: ");
        Serial.println(isMulti);
      });
    }
    //设置滤波
    if(c == 'F') {
      LIDAR07.setMeasureFilter(!LIDAR07.isEnableFilter, [](bool isEnableFilter) {
        Serial.print("set filter: ");
        Serial.println(isEnableFilter);
      });
    }
    //获取 模块错误码
    if(c == 'r') {
      LIDAR07.getMeasureError([](uint8_t errCode) {
        Serial.print("get error: ");
        Serial.println(errCode);
      });
    }
  }
}
