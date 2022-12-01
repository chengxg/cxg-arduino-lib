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
#ifndef FOC_stepper_H
#define FOC_stepper_H

#include "Arduino.h"

class FOCStepper {
private:
  boolean loading = false;
  uint8_t checkByte = 0x6B;
  uint8_t currentId = 1;
  uint8_t readDataLength = 0;
  uint8_t currentCommand = 0;
  // 默认设置参数
  uint8_t buff[10] = {0xAA, 0x5A, 0x00, 0x01, 0x00,
                      0x01, 0x00, 0x00, 0x00, 0x04};
  uint8_t count = 0;

  int (*serialWriteCallback)(const uint8_t *buffer, size_t size) = NULL;
  void (*readDataCallback)(uint32_t data) = NULL;

  int serialWrite(const uint8_t *buffer, size_t size);

public:
  enum FOCStepper_Command {
    OK = 1,
    Error = 2,
    AddressError = 255,
    ReadEncoderNum = 3,
    ReadPulseNum = 4,
    ReadPosition = 5,
    ReadError = 6,
    ReadEnable = 7,
    ReadStall = 8,
    SetEnable = 20,
    SetSpeed = 21, // 控制闭环电机的正反转，即速度模式控制
    // 存储/清除闭环电机正反转，即速度模式当前的参数，上电会自动运行
    SetSpeedParams = 22,
    SetPosition = 23, // 控制闭环电机相对运动的角度，即位置模式控制
    SetCalibration = 24,
    SetZeroposition = 25,
    SetUnlockStall = 26,
    MoveEnable = 30,
    MoveSpeed = 31,
    MovePosition = 32,
  };

  FOCStepper();
  ~FOCStepper();

  void attach(int (*serialWriteCallback)(const uint8_t *buffer, size_t size));

  // 读取数据
  void addData(uint8_t data);

  // 触发编码器校准
  void setCalibration(uint8_t id, void (*callback)(uint32_t data));
  // 下发该命令后，可以将闭环电机的当前电机位置清零，即设置为零点
  void setZeroposition(uint8_t id, void (*callback)(uint32_t data));
  // 下只有触发了堵转保护功能后，下发该命令才有效，可以解除堵转保护
  void setUnlockStall(uint8_t id, void (*callback)(uint32_t data));

  // 读取编码器值, 数据类型为 uint16_t ，范围为0-65535，
  void readEncoderNum(uint8_t id, void (*callback)(uint32_t data));
  // 读取输入脉冲数
  void readPulseNum(uint8_t id, void (*callback)(uint32_t data));
  // 读取电机实时位置
  void readPosition(uint8_t id, void (*callback)(uint32_t data));
  // 读取电机位置误差
  void readError(uint8_t id, void (*callback)(uint32_t data));

  // 控制闭环电机的使能状态
  void moveEnable(uint8_t id, boolean enable, void (*callback)(uint32_t data));
  // 控制闭环电机的正反转，即速度模式控制
  void moveSpeed(uint8_t id, boolean dir, uint16_t speed, uint8_t acceleration,
                 void (*callback)(uint32_t data));
  // 控制闭环电机相对运动的角度，即位置模式控制
  void movePosition(uint8_t id, boolean dir, uint16_t speed,
                    uint8_t acceleration, uint32_t pulseNum,
                    void (*callback)(uint32_t data));
};

#endif
