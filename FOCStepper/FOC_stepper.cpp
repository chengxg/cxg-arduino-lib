/*!
 * @author: chengxg
 * @date: 2022-12-01
 * @description: 张大头 的 Emm42_V4.x 步进闭环驱动器 简单封装
 * @licence The MIT License (MIT)
 * @copyright Copyright (c) 2022 chengxg (https://github.com/chengxg)
 * @url https://github.com/chengxg/cxg-arduino-lib/FOCStepper
 * @version: 1.0.0
 *
 * The MIT License (MIT)
 *  Copyright (c) 2022 by Chengxg
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include "FOC_stepper.h"

FOCStepper::FOCStepper() {}

FOCStepper::~FOCStepper() {}

// 绑定串口发送数据的回调
void FOCStepper::attach(int (*serialWriteCallback)(const uint8_t *buffer,
                                                   size_t size)) {
  this->serialWriteCallback = serialWriteCallback;
}

// 发送数据
int FOCStepper::serialWrite(const uint8_t *buffer, size_t size) {
  int sendNum = 0;
  if (serialWriteCallback) {
    sendNum = serialWriteCallback(buffer, size);
  }
  if (sendNum > 0) {
    count = 0;
    memset(buff, 0, 10);
    readDataCallback = NULL;
  }
  return sendNum;
}

// 读取数据
void FOCStepper::addData(uint8_t data) {
  *(buff + count) = data;
  count++;
  if (count == readDataLength) {
    if (currentId == buff[0] && readDataCallback) {
      if (currentCommand == FOCStepper::ReadEncoderNum && count == 4) {
        //读取编码器值
        uint32_t data = (int32_t)(((int32_t)buff[1] << 8) | (int32_t)buff[2]);
        readDataCallback(data);
      }
      if (currentCommand == FOCStepper::ReadPulseNum && count == 6) {
        //读取输入脉冲数
        uint32_t data =
            (int32_t)(((int32_t)buff[1] << 24) | ((int32_t)buff[2] << 16) |
                      ((int32_t)buff[3] << 8) | ((int32_t)buff[4] << 0));
        readDataCallback(data);
      }
      if (currentCommand == FOCStepper::ReadPosition && count == 6) {
        //读取 电机实时位置
        uint32_t data =
            (int32_t)(((int32_t)buff[1] << 24) | ((int32_t)buff[2] << 16) |
                      ((int32_t)buff[3] << 8) | ((int32_t)buff[4] << 0));
        readDataCallback(data);
      }
      if (currentCommand == FOCStepper::ReadError && count == 4) {
        //读取 位置误差
        uint32_t data = (int32_t)(((int32_t)buff[1] << 8) | (int32_t)buff[2]);
        readDataCallback(data);
      }
      // 通用命令
      if (count == 3 && buff[2] == checkByte) {
        // 收到并确认命令正确
        if (buff[1] == 0x02) {
          readDataCallback(0x02);
        }
        // 收到但确认命令错误
        if (buff[1] == 0xEE) {
          readDataCallback(0xEE);
        }
        // 位置更新完成
        if (buff[1] == 0x9F) {
          readDataCallback(0x9F);
        }
      }
    }
    count = 0;
    memset(buff, 0, 10);
  }
}

// 触发编码器校准
void FOCStepper::setCalibration(uint8_t id, void (*callback)(uint32_t data)) {
  uint8_t cmd[] = {id, 0x06, 0x45, checkByte};
  int num = serialWrite(cmd, 4);
  if (num > 0) {
    this->currentId = id;
    this->readDataCallback = callback;
    this->readDataLength = 3;
    this->currentCommand = FOCStepper::SetCalibration;
  }
}

// 下发该命令后，可以将闭环电机的当前电机位置清零，即设置为零点
void FOCStepper::setZeroposition(uint8_t id, void (*callback)(uint32_t data)) {
  uint8_t cmd[] = {id, 0x0A, 0x6D, checkByte};
  int num = serialWrite(cmd, 4);
  if (num > 0) {
    this->currentId = id;
    this->readDataCallback = callback;
    this->readDataLength = 3;
    this->currentCommand = FOCStepper::SetZeroposition;
  }
}

// 下只有触发了堵转保护功能后，下发该命令才有效，可以解除堵转保护
void FOCStepper::setUnlockStall(uint8_t id, void (*callback)(uint32_t data)) {
  uint8_t cmd[] = {id, 0x0E, 0x52, checkByte};
  int num = serialWrite(cmd, 3);
  if (num > 0) {
    this->currentId = id;
    this->readDataCallback = callback;
    this->readDataLength = 3;
    this->currentCommand = FOCStepper::SetUnlockStall;
  }
}

// 读取编码器值, 数据类型为 uint16_t ，范围为0-65535，
void FOCStepper::readEncoderNum(uint8_t id, void (*callback)(uint32_t data)) {
  uint8_t cmd[] = {id, 0x30, checkByte};
  int num = serialWrite(cmd, 3);
  if (num > 0) {
    this->currentId = id;
    this->readDataCallback = callback;
    this->readDataLength = 4;
    this->currentCommand = FOCStepper::ReadEncoderNum;
  }
}

// 读取输入脉冲数
void FOCStepper::readPulseNum(uint8_t id, void (*callback)(uint32_t data)) {
  uint8_t cmd[] = {id, 0x33, checkByte};
  int num = serialWrite(cmd, 3);
  if (num > 0) {
    this->currentId = id;
    this->readDataCallback = callback;
    this->readDataLength = 6;
    this->currentCommand = FOCStepper::ReadPulseNum;
  }
}

// 读取电机实时位置
void FOCStepper::readPosition(uint8_t id, void (*callback)(uint32_t data)) {
  uint8_t cmd[] = {id, 0x36, checkByte};
  int num = serialWrite(cmd, 3);
  if (num > 0) {
    this->currentId = id;
    this->readDataCallback = callback;
    this->readDataLength = 6;
    this->currentCommand = FOCStepper::ReadPosition;
  }
}

// 读取电机位置误差
void FOCStepper::readError(uint8_t id, void (*callback)(uint32_t data)) {
  uint8_t cmd[] = {id, 0x39, checkByte};
  int num = serialWrite(cmd, 3);
  if (num > 0) {
    this->currentId = id;
    this->readDataCallback = callback;
    this->readDataLength = 4;
    this->currentCommand = FOCStepper::ReadError;
  }
}

// 控制闭环电机的使能状态
void FOCStepper::moveEnable(uint8_t id, boolean enable,
                            void (*callback)(uint32_t data)) {
  uint8_t cmd[] = {id, 0xF3, enable ? 0x01 : 0x00, checkByte};
  int num = serialWrite(cmd, 4);
  if (num > 0) {
    this->currentId = id;
    this->readDataCallback = callback;
    this->readDataLength = 3;
    this->currentCommand = FOCStepper::MoveEnable;
  }
}

// 控制闭环电机的正反转，即速度模式控制
void FOCStepper::moveSpeed(uint8_t id, boolean dir, uint16_t speed,
                           uint8_t acceleration,
                           void (*callback)(uint32_t data)) {
  uint8_t dirByte = dir ? 0x01 : 0x00;
  if (speed > 0x4ff) {
    speed = 0x4ff;
  }
  uint8_t cmd[] = {id,
                   0xF6,
                   (dirByte << 4) | (0x0f & (speed >> 8)),
                   speed & 0x00ff,
                   acceleration,
                   checkByte};
  int num = serialWrite(cmd, 6);
  if (num > 0) {
    this->currentId = id;
    this->readDataCallback = callback;
    this->readDataLength = 3;
    this->currentCommand = FOCStepper::MoveSpeed;
  }
}

// 控制闭环电机相对运动的角度，即位置模式控制
void FOCStepper::movePosition(uint8_t id, boolean dir, uint16_t speed,
                              uint8_t acceleration, uint32_t pulseNum,
                              void (*callback)(uint32_t data)) {
  uint8_t dirByte = dir ? 0x01 : 0x00;
  if (speed > 0x4ff) {
    speed = 0x4ff;
  }
  uint8_t cmd[] = {id,
                   0xFD,
                   (dirByte << 4) | (0x0f & (speed >> 8)),
                   speed & 0x00ff,
                   acceleration,
                   0xff & (pulseNum >> 16),
                   0xff & (pulseNum >> 8),
                   0xff & pulseNum,
                   checkByte};
  int num = serialWrite(cmd, 9);
  if (num > 0) {
    this->currentId = id;
    this->readDataCallback = callback;
    this->readDataLength = 3;
    this->currentCommand = FOCStepper::MovePosition;
  }
}
