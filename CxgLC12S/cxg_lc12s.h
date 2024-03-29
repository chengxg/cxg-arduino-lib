/*!
 * @author: chengxg
 * @date: 2021-01-23
 * @description: LC12S无线模块设置操作库
 * @licence The MIT License (MIT)
 * @copyright Copyright (c) 2022 chengxg (https://github.com/chengxg)
 * @url https://github.com/chengxg/cxg-arduino-lib/CxgLC12S
 * @version: 1.0.0
 * 
 * @version 2.0.0 2022-01-15
 * -- 添加软串口支持的回调
 * 
 * The MIT License (MIT)
 *  Copyright (c) 2022 by Chengxg
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
#ifndef CXG_LC12S_H
#define CXG_LC12S_H

#include "Arduino.h"
#include "cxg_JSTime.h"

class CxgLC12S {
  private:
  //设置波特率
  void setBaud(uint32_t baud);
  void (*hardwareSerialChangeBaudCallback)(HardwareSerial* serial, uint32_t baud) = NULL;

  void serialChangeBaud(uint32_t baud);
  int serialAvailable();
  int serialWrite(const uint8_t* buffer, size_t size);
  int serialRead();

  public:
  uint8_t setpin = 0;
  uint8_t cspin = 0;
  HardwareSerial* serial = NULL;  //串口引用
  boolean isSetting = false;      //是否正在设置LC12S
  //默认设置参数
  uint8_t setBuf[18] = {
    0xAA, 0x5A, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x60, 0x00, 0x00, 0x00, 0x12, 0x00, 0x7C};

  //软串口支持回调
  void (*serialChangeBaudCallback)(uint32_t baud) = NULL;
  int (*serialAvailableCallback)() = NULL;
  int (*serialWriteCallback)(const uint8_t* buffer, size_t size) = NULL;
  int (*serialReadCallback)() = NULL;

  CxgLC12S();
  ~CxgLC12S();

  //在loop循环中不断刷新状态
  static void refresh();

  /**
   * @Description: 绑定串口,初始化参数
   * @param {HardwareSerial* serial} 硬串口引用
   * @param {uint32_t} baud 设置与lc12s的通信波特率
   * @param {uint8_t} setpin 设置引脚, 如果没有绑定设置引脚,那么也就不需要这个库了
   * //因为设置模块时使用9600的波特率,正常通信时是别的波特率,需要动态改变波特率
   * @param {void (*)(HardwareSerial* serial, uint32_t baud)} changeBaud 串口更改波特率的方法, 不同的平台更改方法不一样,需要自定义实现
   * @param {uint8_t} cspin 片选使能引脚, 低电平使能, 可以不绑定使能引脚, 不绑定传255
   */
  void attach(HardwareSerial* serial, uint32_t baud, uint8_t setpin, void (*changeBaud)(HardwareSerial* serial, uint32_t baud), uint8_t cspin = 255);
  /**
   * @Description: 绑定软串口,初始化参数
   * @param {uint32_t} baud 设置与lc12s的通信波特率
   * @param {uint8_t} setpin 设置引脚, 如果没有绑定设置引脚,那么也就不需要这个库了
   * //因为设置模块时使用9600的波特率,正常通信时是别的波特率,需要动态改变波特率
   * @param {uint8_t} cspin 片选使能引脚, 低电平使能, 可以不绑定使能引脚, 不绑定传255
   */
  void attach(uint32_t baud, uint8_t setpin, uint8_t cspin = 255);

  //使能模块
  void enable();
  //禁用模块
  void disable();
  //得到通信波特率
  uint32_t getBaud();
  //设置射频功率 RFPower 1Byte(0~14)
  void setRFPower(uint8_t power);
  //设置射频信道 RF信道 1Byte(0~127) 建议信道不要使用 16 的倍数
  void setRFChannel(uint8_t channel);
  //设置自身id
  void setSelfID(uint16_t id);
  //设置组网id
  void setMeshID(uint16_t id);
  //同步获取模块参数
  void syncParams();
  //设置模块参数
  void setLc12s();
  //获取模块版本
  void getVersion();
};

#endif
