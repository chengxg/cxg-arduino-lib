#include <Arduino.h>
#include "cxg_Command_String.h"

static CxgCommandString command;

void setup() {
  Serial.begin(115200);

  //设置缓冲区大小
  command.setBufferSize(250);
  //设置解析指令回调
  command.setResolveCallback([](char name, char* body, int length) {
    Serial.print("receive command name: ");
    Serial.println(name);
    Serial.print("receive command body: ");
    Serial.println(body);

    if(name == 's') {
      Serial.print("send command: ");
      //发送一个指令
      command.sendCommand('s', "a=hello");
      Serial.println("");
    }
  });

  //设置发送指令回调
  command.setSendCallback([](char* buff, int length) {
    Serial.write(buff);
  });
}

void loop() {
  //串口助手发送  qwe{{d{{a=123|q}}sdf{{b=123|s}}rty 看看
  while(Serial.available()) {
    //接收指令, 直接跟串口结合
    command.addData(Serial.read());
  }
}
