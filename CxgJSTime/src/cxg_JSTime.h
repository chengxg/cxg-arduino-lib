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
 * version 2.0.0 2022-01-15
 * -- 初始化定时器限制数量由10改为20
 * -- 数组开始缩容最小限制改为20
 * -- 将时间比较由 ms 变为 us, 最大延时时间缩小1000倍
 * -- 新增延时时间小数输入, 支持小于1ms的延时
 * -- 复用过时的结构体,避免频繁申请内存
 * -- 新增clearTime重载来支持同时取消多个定时器
 * -- 新增clearAllTime来全部取消定时器
 * 
 *  The MIT License (MIT)
 * 
 *  Copyright (c) 2021-2022 by Chengxg
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
  unsigned long periodTime = 0;
  void (*callback)(int parameterInt, void* parameter) = NULL;
  void (*simpleCallback)() = NULL;
  void* parameter = NULL;
  int parameterInt = 0;
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
  JSTime(int initSize = 1);
  ~JSTime();
  //在loop循环中重复执行
  void refresh();
  //无阻塞延时执行
  //callback: 回调函数
  //time: 延时时间 ms
  int setTimeout(void (*simpleCallback)(), double time);
  //回调函数可以往里传两个参数
  int setTimeout(void (*callback)(int parameterInt, void* parameter), double time, int parameterInt = 0, void* parameter = NULL);
  //无阻塞间隔执行
  //callback: 回调函数
  //time: 延时时间 ms
  int setInterval(void (*simpleCallback)(), double time);
  //回调函数可以往里传两个参数
  int setInterval(void (*callback)(int parameterInt, void* parameter), double time, int parameterInt = 0, void* parameter = NULL);
  //取消执行定时器 必须指定timeId
  //取消成功返回true, 否则false
  bool clearTime(int timeoutId = 0);
  //取消执行,可以取消多个定时器
  bool clearTime(const int* timeoutIds, int start, int length);
  //全部取消定时器
  //取消成功返回true, 否则false
  bool clearAllTime();

  //获取当前活动定时器的总数
  int getNumber();
  //获取所有定时器结构体占用的内存大小
  int getHeapMemorySize();
};

#endif