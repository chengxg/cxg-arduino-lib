/*
 * 受javascript的 setTimeout 和 setInterval启发 实现的延时定时器 
 * 适合延时时间比较长, 延时精度要求不高的场合
 * 创建延时方法返回的id在极限情况下可能会有重复的情况
 * @Author: chengxg
 * @Date: 2021-01-15
 */
#ifndef CXG_JSTIME_H_
#define CXG_JSTIME_H_

#include "Arduino.h"

struct JSTimeStruct {
  unsigned long startTime = 0;
  void (*callback)();
  unsigned long periodTime = 0;
  int id = 0;
  bool isInterval = false;
};

class JSTime {
  private:
  int createTimeoutId = 1;
  int createTimeIntevalId = -1;

  int timeArrLen = 4;
  int timeNum = 0;

  struct JSTimeStruct** timeArr;
  void setSize();

  public:
  JSTime();
  ~JSTime();
  //在loop循环中重复执行
  void refresh();
  //无阻塞延时执行
  //callback: 回调函数
  //time: 延时时间 ms
  int setTimeout(void (*callback)(), unsigned long time);
  //无阻塞间隔执行
  //callback: 回调函数
  //time: 延时时间 ms
  int setInterval(void (*callback)(), unsigned long time);
  //取消执行 如果不指定timeId,则全部取消, 默认全部取消
  bool clearTime(int timeoutId = 0);
};

#endif