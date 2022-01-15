/*
 * 按键库, 检测按键的按下,抬起,长按,双击事件
 * 支持arduino uno, stm32, esp32 
 * @Author: chengxg
 * @Date: 2021-01-15
 * version: 1.0.0
 */
#ifndef CXG_BTN_H_
#define CXG_BTN_H_

#include "Arduino.h"

class CxgBtn {
  private:
  unsigned long btnTempDownStartTime;     //开始按下的时间
  unsigned long btnTempUpStartTime;       //开始按下的时间
  unsigned long btnDownStartTime;         //开始按下的时间
  unsigned long btnUpStartTime;           //开始按下的时间
  unsigned long btnLongPressLastTime;     //上次长按触发的时间
  unsigned long btnDoubleClickStartTime;  //双击开始时间

  uint8_t pin;
  int clickCount = 0;           //按键计数
  boolean activeLevel = true;   //按钮按下是否为低电平
  boolean pullupActive = true;  //输入是否为上拉, 默认为上拉
  boolean isKeydown = false;    //是否按下

  //保存回调引用
  void (*keydownCallback)() = NULL;
  void (*keyupCallback)() = NULL;
  void (*longPressCallback)() = NULL;
  void (*doubleClickCallback)() = NULL;

  void _resetFields();

  public:
  unsigned int btnLongPressInterval = 200;      //长按间隔时间, 每过一段时间触发一次长按回调
  unsigned int btnLongPressTriggerTime = 2000;  //长按触发时间
  unsigned int btnDoubleTriggerTime = 800;      //双击触发时间, 在这个时间内完成点击2次
  unsigned int btnDebounceTime = 20;            //按键防抖延时时间

  CxgBtn();
  ~CxgBtn();

  /**
   * 绑定按钮引脚
   * @Author: chengxg
   * @Date: 2021-01-27
   * @param {uint8_t} pin
   * @param {boolean} activeLevel 什么电平有效, 为:true是高电平有效,为:false是低电平有效,默认true
   * @param {boolean} pullupActive 引脚是否为输入上拉, 默认true
   */
  void attach(uint8_t pin, boolean activeLevel = true, boolean pullupActive = true);

  //循环检查状态
  void check(boolean (*setBtnActive)() = NULL);

  //按键按下回调
  void setBtnKeydown(void (*keydown)());
  //按键抬起回调
  void setBtnKeyUp(void (*keyup)());
  //按键长按回调
  void setBtnLongPress(void (*longPress)());
  //按键双击回调
  void setBtnDoubleClickCallback(void (*doubleClickCallback)());
};
#endif