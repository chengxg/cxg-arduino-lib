/*!
 * @author: chengxg
 * @date: 2022-01-02
 * @description: TP_SOLAR LIDAR07-100W-B 测距模块
 * @licence The MIT License (MIT)
 * @copyright Copyright (c) 2022 chengxg (https://github.com/chengxg)
 * @url https://github.com/chengxg/cxg-arduino-lib/CxgLIDAR07
 * version: 1.0.0
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
#include "cxg_LIDAR07.h"

#define IS_DEBUG_LIDAR07

#define LIDAR07_IIC_ADDR 0x70   //默认IIC地址
#define SEND_HEAD0 0xF5         //主机发送帧头
#define READ_HEAD0 0xFA         //模块响应帧头
#define READ_LENGTH 12          //返回数据的长度
#define READ_MEASURE_LENGTH 24  //返回测量结果数据的长度

#define COMMAND_TYPE_READ 0x00   //读取指令
#define COMMAND_TYPE_WRITE 0x80  //写入指令

#define COMMAND_VERSION 0x43           //返回模组批次的版本号
#define COMMAND_MEASURE_FILTER 0x59    //滤波的使用
#define COMMAND_MEASURE 0x60           //距离测量开关 打开：1 关闭：0
#define COMMAND_MEASURE_MODE 0x61      //测距模式的选择 选择单次测量和连续测量
#define COMMAND_MEASURE_INTERVAL 0x62  //测量频率 在连续测量模式下，可以设定连续测量的频率。
#define COMMAND_ERR_CODE 0x65          //系统运行错误状态 模块运行过程中的错误状态查询

CxgLIDAR07::CxgLIDAR07() {
  setBufferSize(25);
  isReadData = true;
}

CxgLIDAR07::~CxgLIDAR07() {
  if(buff != NULL) {
    free(buff);
  }
  buff = NULL;
  serial = NULL;
  wire = NULL;
}

bool CxgLIDAR07::setBufferSize(uint8_t size) {
  if(buff != NULL) {
    free(buff);
    buff = NULL;
  }
  buff = ( byte* )malloc(sizeof(byte) * size);
  if(buff == NULL) {
    return false;
  }
  buffSize = size;
  memset(buff, 0, size);
  return true;
}

//计算CRC32
uint32_t CxgLIDAR07::calcCRC32(uint8_t* data, int start, int len) {
  uint32_t crc = 0xFFFFFFFF;
  uint32_t temp = 0;
  int end = start + len;
  for(int i = start; i < end; i++) {
    temp = data[i];
    crc = crc ^ (temp << 24);
    for(uint8_t y = 0; y < 8; y++) {
      if(crc & 0x80000000) {
        crc = (crc << 1) ^ 0x04C11DB7;
      } else {
        crc <<= 1;
      }
    }
  }
  crc = crc ^ 0x00;
  return crc;
}

//拼接上CRC32
uint32_t CxgLIDAR07::appendCRC32(uint8_t* data, int start, int len) {
  uint32_t crc = 0;
  if(isUseUart) {
    crc = calcCRC32(data, start, len);
  }
  if(isUseIIC) {
    crc = calcCRC32(data, start + 1, len - 1);
  }

  *(data + len) = crc;
  *(data + len + 1) = (crc >> 8);
  *(data + len + 2) = (crc >> 16);
  *(data + len + 3) = (crc >> 24);
  return crc;
}

//验证数据的CRC
bool CxgLIDAR07::checkCRC32(uint8_t* data, int start, int len) {
  uint32_t crc = calcCRC32(data, start, len - 4);
  uint32_t checkCrc = ( uint32_t )data[len - 4] | (( uint32_t )data[len - 3] << 8) | (( uint32_t )data[len - 2] << 16) | (( uint32_t )data[len - 1] << 24);
  return crc == checkCrc;
}

void CxgLIDAR07::refresh() {
  if(!isReadData) {
    handleData();
    return;
  }
  if(serial != NULL) {
    while(serial->available()) {
      bool isReceived = addData(serial->read());
      if(isReceived) {
        break;
      }
    }
  }
  // if(wire != NULL && dataReadyPin > 0) {
  if(wire != NULL && isReadIICData) {
    uint8_t ret = wire->requestFrom(( uint8_t )wireDeviceAddr, ( uint8_t )READ_MEASURE_LENGTH);
    if(ret > 0) {
#ifdef IS_DEBUG_LIDAR07
      Serial.println("start read IIC:");
#endif
      count = 0;
      addData(READ_HEAD0);
      while(wire->available()) {
        bool isReceived = addData(wire->read());
        if(isReceived) {
          break;
        }
      }
      while(wire->available()) {
        wire->read();
      }
      wire->flush();
    }
    isReadIICData = false;
  }
}

void CxgLIDAR07::handleData() {
  isReadData = true;
  count = 0;
  uint8_t command = *(buff + 1);
  if(command == (COMMAND_MEASURE | COMMAND_TYPE_WRITE)) {
    //返回测量结果
    distance = *(buff + 4) + *(buff + 5) * 256;
    uint16_t temperature = *(buff + 6) + *(buff + 7) * 256;
    uint16_t amplitude = *(buff + 8) + *(buff + 9) * 256;
    uint16_t backgroundLight = *(buff + 10) + *(buff + 11) * 256;
    if(measureDistanceCallback != NULL) {
      measureDistanceCallback(distance, temperature, amplitude, backgroundLight);
    }
    return;
  }

  //版本
  if(command == COMMAND_VERSION) {
    char version[6] = {( char )(*(buff + 6) + '0'), '.', ( char )(*(buff + 5) + '0'), '.', ( char )(*(buff + 4) + '0'), 0};
    if(sendVersionCallback != NULL) {
      sendVersionCallback(version);
      sendVersionCallback = NULL;
    }
    return;
  }

  //测量模式
  if(command == COMMAND_MEASURE_MODE || command == (COMMAND_MEASURE_MODE | COMMAND_TYPE_WRITE)) {
    isMeasureMulti = *(buff + 4);
    if(measureModeCallback != NULL) {
      measureModeCallback(isMeasureMulti);
      measureModeCallback = NULL;
    }
    return;
  }

  //测量间隔时间
  if(command == COMMAND_MEASURE_INTERVAL || command == (COMMAND_MEASURE_INTERVAL | COMMAND_TYPE_WRITE)) {
    measureInterval = *(buff + 4) + *(buff + 5) * 256;
    if(measureIntervalCallback != NULL) {
      measureIntervalCallback(measureInterval);
      measureIntervalCallback = NULL;
    }
    return;
  }

  //测量是否滤波
  if(command == COMMAND_MEASURE_FILTER || command == (COMMAND_MEASURE_FILTER | COMMAND_TYPE_WRITE)) {
    isEnableFilter = *(buff + 4);
    if(measureFilterCallback != NULL) {
      measureFilterCallback(isEnableFilter);
      measureFilterCallback = NULL;
    }
    return;
  }

  //测量模块错误
  if(command == COMMAND_ERR_CODE) {
    if(measureErrCallback != NULL) {
      measureErrCallback(*(buff + 4));
      measureErrCallback = NULL;
    }
    return;
  }
}

bool CxgLIDAR07::addData(uint8_t data) {
#ifdef IS_DEBUG_LIDAR07
  Serial.print(data, HEX);
  Serial.print(" ");
#endif
  //开始
  if(count == 0 && data == READ_HEAD0) {
    *buff = data;
    count = 1;
    return false;
  }

  if(count >= buffSize) {
    count = 0;
    return false;
  }

  *(buff + count) = data;
  count++;

  if(count == READ_LENGTH && *(buff + 1) != 0xE0) {  // COMMAND_MEASURE | COMMAND_TYPE_WRITE
    if(!checkCRC32(buff, 0, READ_LENGTH)) {
      return false;
    }
    isReadData = false;
    return true;
  }
  if(count == READ_MEASURE_LENGTH && *(buff + 1) == 0xE0) {  // COMMAND_MEASURE | COMMAND_TYPE_WRITE
    if(!checkCRC32(buff, 0, READ_MEASURE_LENGTH)) {
      return false;
    }
    isReadData = false;
    return true;
  }
  return false;
}

void CxgLIDAR07::attachUart(HardwareSerial* serial) {
  isReadData = true;
  if(serial) {
    this->serial = serial;
    isUseUart = true;
    wire = NULL;
    isUseIIC = false;
  } else {
    this->serial = NULL;
    isUseUart = false;
    dataReadyPin = 0;
  }
}

void CxgLIDAR07::attachWire(TwoWire* wire, int8_t dataReadyPin, uint8_t wireDeviceAddr) {
  isReadData = true;
  if(wire) {
    this->wire = wire;
    isUseIIC = true;
    serial = NULL;
    isUseUart = false;
    if(wireDeviceAddr == 0) {
      wireDeviceAddr = LIDAR07_IIC_ADDR;
    }
    this->wireDeviceAddr = wireDeviceAddr;
    if(dataReadyPin > 0) {
      this->dataReadyPin = dataReadyPin;
      pinMode(dataReadyPin, INPUT);
    }
  } else {
    this->wire = NULL;
    isUseIIC = false;
  }
}

void CxgLIDAR07::uartWrite(byte* data, int start, int length) {
  if(!serial) {
    return;
  }
  if(start == 0 && length > 1) {
    serial->write(data, length);
#ifdef IS_DEBUG_LIDAR07
    Serial.println();
    Serial.print("send uart data: ");
    for(int i = start; i < start + length; i++) {
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
#endif
  } else {
    int end = start + length;
    for(int i = start; i < end; i++) {
      serial->write(data[i]);
    }
  }
}

void CxgLIDAR07::wireWrite(byte* data, int start, int length) {
  if(!wire) {
    return;
  }
  while(wire->available()) {
    wire->read();
  }
  wire->beginTransmission(wireDeviceAddr);
  int end = start + length;
#ifdef IS_DEBUG_LIDAR07
  Serial.println("send IIC data:");
#endif
  for(int i = start; i < end; i++) {
    wire->write(data[i]);
#ifdef IS_DEBUG_LIDAR07
    Serial.print(data[i], HEX);
    Serial.print(" ");
#endif
  }
  wire->endTransmission();
#ifdef IS_DEBUG_LIDAR07
  Serial.println("");
#endif
}

void CxgLIDAR07::sendData(byte* data, int start, int length) {
  if(sendDataCallback) {
    sendDataCallback(data, start, length);
    return;
  }
  if(isUseUart) {
    uartWrite(data, start, length);
    return;
  }
  if(isUseIIC) {
    //用IIC 包头就不用发了
    wireWrite(data, start + 1, length - 1);
    return;
  }
}

//-----------------------------交互接口-------------------------------
void CxgLIDAR07::getVersion(void (*callback)(char* version)) {
  byte command[10] = {SEND_HEAD0, COMMAND_VERSION | COMMAND_TYPE_READ, 0x00, 0x00, 0x00, 0x00};
  appendCRC32(command, 0, 6);
  sendData(command, 0, 10);
  sendVersionCallback = callback;
}

void CxgLIDAR07::setMeasureInterval(uint16_t measureInterval, void (*callback)(uint16_t measureInterval)) {
  byte command[10] = {SEND_HEAD0, COMMAND_MEASURE_INTERVAL | COMMAND_TYPE_WRITE, (byte)(measureInterval & 0x00FF), (byte)((measureInterval & 0xFF00) >> 8), 0x00, 0x00};
  appendCRC32(command, 0, 6);
  sendData(command, 0, 10);
  measureIntervalCallback = callback;
}

void CxgLIDAR07::setMeasureMode(bool isMulti, void (*callback)(bool isMulti)) {
  byte command[10] = {SEND_HEAD0, COMMAND_MEASURE_MODE | COMMAND_TYPE_WRITE, isMulti ? ( byte )1 : ( byte )0, 0x00, 0x00, 0x00};
  appendCRC32(command, 0, 6);
  sendData(command, 0, 10);
  measureModeCallback = callback;
}

void CxgLIDAR07::startMeasure() {
  byte command[10] = {SEND_HEAD0, COMMAND_MEASURE | COMMAND_TYPE_WRITE, 0x01, 0x00, 0x00, 0x00};
  appendCRC32(command, 0, 6);
  sendData(command, 0, 10);
}

void CxgLIDAR07::stopMeasure() {
  byte command[10] = {SEND_HEAD0, COMMAND_MEASURE | COMMAND_TYPE_WRITE, 0x00, 0x00, 0x00, 0x00};
  appendCRC32(command, 0, 6);
  sendData(command, 0, 10);
}

void CxgLIDAR07::setMeasureFilter(bool isEnable, void (*callback)(bool isEnableFilter)) {
  byte command[10] = {SEND_HEAD0, COMMAND_MEASURE_FILTER | COMMAND_TYPE_WRITE, isEnable ? ( byte )1 : ( byte )0, 0x00, 0x00, 0x00};
  appendCRC32(command, 0, 6);
  sendData(command, 0, 10);
  measureFilterCallback = callback;
}

void CxgLIDAR07::getMeasureError(void (*callback)(uint8_t errCode)) {
  byte command[10] = {SEND_HEAD0, COMMAND_ERR_CODE | COMMAND_TYPE_READ, 0x00, 0x00, 0x00, 0x00};
  appendCRC32(command, 0, 6);
  sendData(command, 0, 10);
  measureErrCallback = callback;
}

void CxgLIDAR07::setMeasureDistanceCallback(void (*callback)(uint16_t distance, uint16_t temperature, uint16_t amplitude, uint16_t backgroundLight)) {
  measureDistanceCallback = callback;
}