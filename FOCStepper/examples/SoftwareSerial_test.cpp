#include <Arduino.h>

// 依赖我的JSTime库
#include "cxg_JSTime.h"
#include "FOC_stepper.h"
#include "SoftwareSerial.h"  //https://github.com/plerup/espsoftwareserial/

#define rx_pin 26
#define tx_pin 25

// esp32 demo

static JSTime jsTime;
static FOCStepper stepper;
static SoftwareSerial sSerial;

void setup() {
  Serial.begin(115200);

  sSerial.begin(115200, SWSERIAL_8N1, rx_pin, tx_pin, false, 256);
  stepper.attach([](const uint8_t* buffer, size_t size) -> int {
    return sSerial.write(buffer, size);
  });

  Serial.println("setup");

  jsTime.setInterval([]() {
    stepper.movePosition(0x01, true, 0x4ff, 0xff, 800, [](uint32_t data) {
      if(data == 0x02) {
        Serial.println("move success");
      }
      if(data == 0xEE) {
        Serial.println("move fail");
      }
    });
  },
    1000);
}

void loop() {
  jsTime.refresh();
  // 读取收到的串口数据
  while(sSerial.available()) {
    stepper.addData(sSerial.read());
  }
}
