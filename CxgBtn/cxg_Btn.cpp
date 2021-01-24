#include "cxg_Btn.h"

/**
 * 绑定按钮引脚
 * @Author: chengxg
 * @Date: 2021-01-23
 * @param {uint8_t} pin
 * @param {boolean} activeLevel 什么电平有效, 为:true是高电平有效,为:false是低电平有效,默认true
 * @param {boolean} pullupActive 引脚是否为输入上拉, 默认true
 */
CxgBtn::CxgBtn(uint8_t pin, boolean activeLevel, boolean pullupActive) {
  this->pin = pin;
  this->activeLevel = activeLevel;
  this->pullupActive = pullupActive;

  if(pullupActive) {
    pinMode(pin, INPUT_PULLUP);
  } else {
    pinMode(pin, INPUT);
  }
}

CxgBtn::~CxgBtn() {
  _resetFields();
}

//在loop循环中不断检查按钮状态
void CxgBtn::check() {
  //是否按下
  boolean isActive = digitalRead(pin) == activeLevel;
  unsigned long t = millis();  //当前时间

  //按下
  if(isActive && !isKeydown) {
    if(btnTempDownStartTime == 0) {
      btnTempDownStartTime = t;
    }
    //延时20ms, 防抖
    if(t - btnTempDownStartTime > btnDebounceTime) {
      btnTempDownStartTime = 0;
      btnDownStartTime = t;
      isKeydown = true;
      clickCount++;
      //记录双击开始时间
      if(clickCount == 1) {
        btnDoubleClickStartTime = t;
      }
      if(keydownCallback != NULL) {
        keydownCallback();
      }
    }
    return;
  }

  //长按
  if(isActive && isKeydown && longPressCallback != NULL) {
    //达到长按触发时间, 进入长按
    if(t - btnDownStartTime > btnLongPressTriggerTime) {
      //每隔btnLongPressInterval, 触发一次长按事件
      if(t - btnLongPressLastTime > btnLongPressInterval) {
        btnLongPressLastTime = t;
        longPressCallback();
      }
    } else {
      btnLongPressLastTime = 0;
    }
    return;
  }

  //抬起
  if(!isActive && isKeydown) {
    if(btnTempUpStartTime == 0) {
      btnTempUpStartTime = t;
    }
    //延时20ms, 防抖
    if(btnTempUpStartTime > 0 && t - btnTempUpStartTime > btnDebounceTime) {
      btnTempUpStartTime = 0;
      btnLongPressLastTime = 0;
      isKeydown = false;
      //第二次单击抬起, 检测是否为双击
      if(clickCount == 2) {
        clickCount = 0;
        if(doubleClickCallback != NULL && t - btnDoubleClickStartTime < btnDoubleTriggerTime) {
          doubleClickCallback();
        }
      }
      //如果一次单击时间超长,那么双击无效
      if(t - btnDoubleClickStartTime > btnDoubleTriggerTime / 2) {
        clickCount = 0;
      }
      //单击抬起
      if(keyupCallback != NULL) {
        keyupCallback();
      }
    }
    return;
  }
}

void CxgBtn::setBtnKeydown(void (*keydown)()) {
  this->keydownCallback = keydown;
}

void CxgBtn::setBtnKeyUp(void (*keyup)()) {
  this->keyupCallback = keyup;
}

void CxgBtn::setBtnLongPress(void (*longPress)()) {
  this->longPressCallback = longPress;
}

void CxgBtn::setBtnDoubleClickCallback(void (*doubleClickCallback)()) {
  this->doubleClickCallback = doubleClickCallback;
}

void CxgBtn::_resetFields(void) {
  isKeydown = false;
}
