#include <Arduino.h>
#include "cxg_Command.h"

static CxgCommand command;

void setup() {
  Serial.begin(115200);
  delay(1000);

  //设置缓冲区大小, 预计指令的最大的长度
  command.setBufferSize(20);
  byte start[2] = {0xff, 0xfe};
  //设置开始指令开始的匹配
  command.setStart(start, 2);
  byte end[2] = {0xfd, 0xfc};
  //设置开始指令结束的匹配
  command.setEnd(end, 2);

  //设置指令处理回调
  command.setResolveCommandCallback([](byte* buff, int startIndex, int length) {
    int endIndex = startIndex + length;
    Serial.println("receive Command: ");

    for(int i = startIndex; i < endIndex; i++) {
      Serial.print(*(buff + i), HEX);
      Serial.print(" ");
    }
    Serial.println("\n");
  });

  //设置发送数据的回调实现
  command.setSendCommandCallback([](byte* buff, int length) {
    //通过串口发送
    Serial.write(buff, length);
  });

  int size = 22;
  byte data[size] = {0x45, 0x56, 0xff, 0xfe, 0xf4, 0x01, 0x01, 0x03,
    0xfd, 0xfc, 0x35, 0x56, 0xff, 0xfe, 0x01, 0xf4,
    0x21, 0xfe, 0x02, 0x05, 0xfd, 0xfc};
  for(int i = 0; i < size; i++) {
    //往里添加数据
    command.addData(data[i]);
  }

  //发送指令
  byte selfData[5] = {0x45, 0x56, 0xff, 0xfe, 0xf0};
  // command.sendCommand(selfData, 5);
}

void loop() {
  while(Serial.available()) {
    //接收指令, 直接跟串口结合
    command.addData(Serial.read());
  }
}
