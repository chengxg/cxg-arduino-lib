/*
 * 自定义通信指令解析库, 专门解析固定格式的字符串
 * 支持arduino uno, stm32, esp32 
 * 从一串二进制数组中解析符合以下格式的数据
 * 解析格式 {{param=value&param1=value1(|v(验证码一个字符))|n(指令名称,一个字符)}}
 * @Author: chengxg
 * @Date: 2021-03-17
 * version: 1.0.0
 * 
 * 解析格式变为 {param=value&param1=value1|n(指令名称,一个字符)|vv(验证码两个字符, 可为空)}
 * version: 2.0.0
 * @Date: 2022-01-02
 */
#ifndef CXG_COMMAND_STRING_H_
#define CXG_COMMAND_STRING_H_

#include "Arduino.h"

class CxgCommandString {
  private:
  byte* buff = NULL;
  int buffSize = 0;
  int count = 0;

  void (*resolveCallback)(char name, char* body, int length) = NULL;
  void (*sendCallback)(char* buff, int length) = NULL;
  bool (*forwardCallback)(char name, char* buff, int length) = NULL;

  void resolveCommand(int length);

  public:
  CxgCommandString();
  ~CxgCommandString();

  //设置缓冲区大小
  bool setBufferSize(unsigned int size);
  //设置解析回调
  void setResolveCallback(void (*resolveCallback)(char name, char* body, int length));
  //设置发送数据回调
  void setSendCallback(void (*sendCallback)(char* buff, int length));
  //设置转发回调
  void setForwardCallback(bool (*forwardCallback)(char name, char* buff, int length));

  //发送指令
  void sendCommand(char name, const char* body, bool isVerify = true);

  //往里添加数据进行解析
  void addData(byte data);

  char* getQueryParam(char* des, const char* urlQuery, const char* paramsName);
  int getQueryParamToInt(const char* urlQuery, const char* paramsName);
  void strcatInt(char* des, int num);
  void resError(char name, int id, const char* errMsg);
  void resSuccess(char name, int id, const char* data);
};
#endif