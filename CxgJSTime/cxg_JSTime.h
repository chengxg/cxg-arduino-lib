/*
 * 受javascript的 setTimeout 和 setInterval启发 实现的延时定时器 
 * 适合延时时间比较长, 延时精度要求不高的场合
 * github: https://github.com/chengxg/cxg-arduino-lib/tree/master/CxgJSTime
 * @Author: chengxg
 * @Date: 2021-01-15
 * version 1.0.0
 */
#ifndef CXG_JSTIME_H_
#define CXG_JSTIME_H_

#include "Arduino.h"

struct JSTimeStruct {
  unsigned long startTime = 0;
  void (*callback)(int parameterInt, void* parameter);
  void* parameter;
  int parameterInt;
  void (*simpleCallback)();
  unsigned long periodTime = 0;
  int id = 0;
  bool isInterval = false;
};

class JSTime {
  private:
  int createTimeoutId = 1;
  int createTimeIntevalId = -1;

  int arrLen = 1;
  int num = 0;

  struct JSTimeStruct** arr;
  bool setSize();
  int baseSetTimeout(void (*simpleCallback)(), void (*callback)(int parameterInt, void* parameter), unsigned long time, int parameterInt, void* parameter);
  int baseSetInterval(void (*simpleCallback)(), void (*callback)(int parameterInt, void* parameter), unsigned long time, int parameterInt, void* parameter);

  public:
  //预先初始化的数量,默认为1个
  JSTime(int intSize = 1);
  ~JSTime();
  //在loop循环中重复执行
  void refresh();
  //无阻塞延时执行
  //callback: 回调函数
  //time: 延时时间 ms
  int setTimeout(void (*simpleCallback)(), unsigned long time);
  //回调函数可以往里传两个参数
  int setTimeout(void (*callback)(int parameterInt, void* parameter), unsigned long time, int parameterInt = 0, void* parameter = NULL);
  //无阻塞间隔执行
  //callback: 回调函数
  //time: 延时时间 ms
  int setInterval(void (*simpleCallback)(), unsigned long time);
  //回调函数可以往里传两个参数
  int setInterval(void (*callback)(int parameterInt, void* parameter), unsigned long time, int parameterInt = 0, void* parameter = NULL);
  //取消执行 如果不指定timeId,则全部取消, 默认全部取消
  //取消成功返回true, 否则false
  bool clearTime(int timeoutId = 0);

  //获取活动定时器的总数
  int getNumber();
  //获取所有定时器结构体占用的内存大小
  int getHeapMemorySize();
};

#endif