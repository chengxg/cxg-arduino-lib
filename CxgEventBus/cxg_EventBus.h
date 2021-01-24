/*
 * 类似于javascript的事件总线
 * @Author: chengxg
 * @Date: 2021-01-24
 */
#ifndef CXG_EventBus_H_
#define CXG_EventBus_H_

#include "Arduino.h"

struct EventBusStruct {
  char* name;
  bool isOnce = false;  //是否只执行一次
  void (*callback)(void* parameter);
};

class CxgEventBus {
  private:
  int eventBusArrLen = 4;
  int eventBusNum = 0;

  struct EventBusStruct** eventBusArr;
  void setSize();
  bool CxgEventBus::addEvent(char* name, void (*callback)(void* parameter), bool isOnce, bool cover);

  public:
  CxgEventBus();
  ~CxgEventBus();
  //绑定事件
  //name: 事件名
  //callback: 执行函数
  //是否覆盖之前的事件, 默认false,不覆盖
  //创建失败则返回 false
  bool on(char* name, void (*callback)(void* parameter), bool cover = false);
  //绑定事件. 只执行一次
  //其他参数同上
  bool once(char* name, void (*callback)(void* parameter), bool cover = false);
  //触发事件
  //name: 事件名
  //parameter: 参数, 默认没有
  void emit(char* name, void* parameter = NULL);
  //移除事件, 只有name和callback都相同才能取消
  //name: 事件名
  //callback: 回调函数
  void off(char* name, void (*callback)(void* parameter));
  //清空所有事件
  void clear();
};

#endif