#include <Arduino.h>
#include "cxg_serial_command.h"

/**
 * 解析指令
 * 指令格式 "{name, param1=value1&param2=value2, 12345, 2321}"
 */
static void resolveCommandData(char* name, char* body, char* time, int verify,
  HardwareSerial serial) {
  //验证计算校验和
  if(verify > 0) {
    int bodyVerify = calcStrSum(body);
    if(bodyVerify != verify) {
      return;
    }
  }

  // strcmp返回值：
  // 如果返回值 < 0，则表示 str1 小于 str2。
  // 如果返回值 > 0，则表示 str2 小于 str1。
  // 如果返回值 = 0，则表示 str1 等于 str2。
  if(strcmp(name, "s") == 0) {
    int data1Int = getCommandParamToInt(body, "param1");
    if(data1Int < INT32_MAX) {
      // TODO
    }
    int data2Int = getCommandParamToInt(body, "param2");
    if(data2Int < INT32_MAX) {
      // TODO
    }
  }
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  readCommandData(Serial3, resolveCommandData);
}
