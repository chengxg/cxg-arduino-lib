/*!
 * @author: chengxg
 * @date: 2022-01-02
 * @description: TP_SOLAR LIDAR07-100W-B 测距模块
 * @licence The MIT License (MIT)
 * @copyright Copyright (c) 2022 chengxg (https://github.com/chengxg)
 * @url https://github.com/chengxg/cxg-arduino-lib/CxgLIDAR07
 * @version: 1.0.0
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
#ifndef CXG_LIDAR07_H
#define CXG_LIDAR07_H

#include "Arduino.h"
#include <Wire.h>

class CxgLIDAR07 {
  private:
  uint8_t* buff = NULL;
  int buffSize = 25;
  int count = 0;

  bool isUseUart = false;
  bool isUseIIC = false;
  bool isReadData = true;
  bool lastIICDataReadyState = false;

  HardwareSerial* serial = NULL;
  TwoWire* wire = NULL;
  uint8_t wireDeviceAddr = 0;  //设备地址
  int8_t dataReadyPin = 0;     //IIC数据准备好 通知引脚

  //分配内存
  bool setBufferSize(uint8_t size);
  void handleData();
  uint32_t appendCRC32(uint8_t* data, int start, int len);
  uint32_t calcCRC32(uint8_t* data, int start, int len);
  bool checkCRC32(uint8_t* data, int start, int len);

  void uartWrite(byte* data, int start, int length);
  void wireWrite(byte* data, int start, int length);
  void sendData(byte* data, int start, int length);
  //设置发送数据实现 用于 SoftwareSerial软串口 等的发送实现
  void (*sendDataCallback)(byte* data, int start, int length) = NULL;

  //查看版本返回
  void (*sendVersionCallback)(char* version) = NULL;
  //设置测量模式返回, true:主动测量, false:单次测量
  void (*measureModeCallback)(bool isMulti) = NULL;
  //设置测量频率返回 单位ms
  void (*measureIntervalCallback)(uint16_t measureInterval) = NULL;
  //是否开启滤波返回 true:开启滤波, false:关闭滤波
  void (*measureFilterCallback)(bool isEnableFilter) = NULL;
  // 0x00000001 ：SPI 通讯出错
  // 0x00000002 ：像素饱和
  // 0x00000004 ：采样上溢出
  // 0x00000008 ：采用下溢出
  // 0x00000010 ：内部测距异常
  // 0x00000020 ：信号太强
  // 0x00000040 ：信号太弱
  void (*measureErrCallback)(uint8_t errCode) = NULL;
  //测距结果返回
  void (*measureDistanceCallback)(uint16_t distance, uint16_t temperature, uint16_t amplitude, uint16_t backgroundLight) = NULL;

  public:
  bool isReadIICData = false;
  bool isMeasureMulti = false;   //是否主动多次测量
  bool isEnableFilter = false;   //是否启用滤波
  uint16_t measureInterval = 100;  //测量间隔 ms
  uint16_t distance = 0;         //测量距离 mm

  CxgLIDAR07();
  ~CxgLIDAR07();

  //在loop循环中不断刷新状态
  void refresh();
  //接收数据
  bool addData(uint8_t data);
  //启用uart模式
  void attachUart(HardwareSerial* serial);
  //启用IIC模式
  void attachWire(TwoWire* wire, int8_t dataReadyPin, uint8_t wireDeviceAddr);

  //获取模块版本
  void getVersion(void (*callback)(char* version));
  //设置测距间隔
  void setMeasureInterval(uint16_t measureInterval, void (*callback)(uint16_t measureInterval));
  //当前测距模式  true:单次测量 false:连续测量
  void setMeasureMode(bool isMulti, void (*callback)(bool isMulti));
  //启用或关闭滤波
  void setMeasureFilter(bool isEnable, void (*callback)(bool isEnableFilter));
  //获取模块错误
  void getMeasureError(void (*callback)(uint8_t errCode));

  //开始测量
  void startMeasure();
  //关闭测量
  void stopMeasure();
  //设置测量结果回调
  void setMeasureDistanceCallback(void (*callback)(uint16_t distance, uint16_t temperature, uint16_t amplitude, uint16_t backgroundLight));
};

#endif
