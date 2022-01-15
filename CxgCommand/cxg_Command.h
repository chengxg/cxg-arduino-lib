/*
 * 自定义通信指令解析库
 * 支持arduino uno, stm32, esp32 
 * 从一串二进制数组中解析符合以下格式的数据, 发送实际数据内容长度最多255个数据
 * (帧头)+实际数据+数据长度(一个字节)+(帧尾)
 * @Author: chengxg
 * @Date: 2021-02-27
 * version: 1.0.0
 */
#ifndef CXG_COMMAND_H_
#define CXG_COMMAND_H_

#include "Arduino.h"

class CxgCommand {
  private:
  byte* startBuff = NULL;
  int startSize = 0;
  byte* endBuff = NULL;
  int endSize = 0;
  byte* buff = NULL;
  int buffSize = 0;
  int count = 0;
  bool isStart = false;

  void (*resolveCommandCallback)(byte* buff, int startIndex, int length) = NULL;
  void (*resolveCommandParamCallback)(byte* buff, int startIndex, int length, void* param) = NULL;
  void* resolveCommandCallbackParameter = NULL;

  void (*sendCommandCallback)(byte* buff, int length) = NULL;

  bool checkCommand(int checkIndex);
  bool isMatchStart(int compareIndex);
  bool isMatchEnd(int compareIndex);

  public:
  CxgCommand();
  ~CxgCommand();

  //设置帧头
  void setStart(byte* st, unsigned int size);
  //设置帧尾
  void setEnd(byte* ed, unsigned int size);
  //设置缓冲大小
  bool setBufferSize(unsigned int size);
  //设置解析到数据的回调
  void setResolveCommandCallback(void (*resolveCommand)(byte* buff, int startIndex, int length));
  void setResolveCommandParamCallback(void (*resolveCommand)(byte* buff, int startIndex, int length, void* param), void* param);
  //设置发送数据的回调实现方式, 比如通过串口发送,i2c,spi发送等
  void setSendCommandCallback(void (*sendCommand)(byte* buff, int length));
  //往里添加数据进行解析
  void addData(byte data);
  //发送指令
  void sendCommand(byte* command, int length);
  //计算校验和
  uint32_t getVerifySum(byte* data, int start, int length);
};
#endif