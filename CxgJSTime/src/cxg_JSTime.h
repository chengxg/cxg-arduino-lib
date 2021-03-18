/*
 * 受javascript的 setTimeout 和 setInterval启发 实现的延时定时器 
 * 适合延时时间比较长, 延时精度要求不高的场合
 * github: https://github.com/chengxg/CxgJSTime
 * @Author: chengxg
 * @Date: 2021-01-15
 * version 1.0.0
 * 
 * version 1.1.0 2021-03-15
 * -- 修复嵌套执行时引发的某些bug
 * 
 *  The MIT License (MIT)
 * 
 *  Copyright (c) 2021 by Chengxg
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */
#ifndef CXG_JSTIME_H_
#define CXG_JSTIME_H_

#include "Arduino.h"

struct JSTimeStruct {
  unsigned long startTime = 0;
  void (*callback)(int parameterInt, void* parameter) = NULL;
  void* parameter = NULL;
  int parameterInt = 0;
  void (*simpleCallback)() = NULL;
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

  struct JSTimeStruct** arr = NULL;
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