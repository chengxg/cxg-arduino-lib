/*
 *NEC红外遥控编码
 *不使用额外的定时来进行时间测量, 仅使用1个支持外部中断的即可。
 *原理, 利用上升沿中断记录两次上升沿时间间隔来判断
 *引导码, 重复码, 数字1和数字0
 */
#ifndef CXG_IRremote_H
#define CXG_IRremote_H

#include "Arduino.h"

class CxgIRremote {
  private:
  uint8_t pin;
  uint8_t receiveIndex = 0;                      //当前接收的索引
  uint8_t IR_buf[4] = {0x00, 0x00, 0x00, 0x00};  //接收缓存数组
  boolean isReceiving = false;                   //是否正在接收
  unsigned long lastRisingTime = 0;              //上次上升沿中断时间

  void _resetFields();

  public:
  CxgIRremote();
  ~CxgIRremote();
  void attach(uint8_t pin);
  //绑定红外遥控上升沿中断
  void handleRisingInterrupt();
  //在循环中读取红外遥控是否接收到了数据,校验反码, 只返回两个字节
  uint16_t read();
  //有的遥控器没有反码,就不校验了,直接返回32位数据
  //在循环中检查是否获取到了数据,没获取到返回0
  uint32_t read32();
};

#endif
