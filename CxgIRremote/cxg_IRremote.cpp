#include "cxg_IRremote.h"

static uint8_t bitReverse(uint8_t src) {
  uint8_t retValue = 0;
  int i;
  retValue |= (src & 1);
  for(i = 1; i < 8; i++) {
    retValue <<= 1;
    retValue |= (((1 << i) & src) >> i);
  }
  return retValue;
}

CxgIRremote::CxgIRremote() {
}

CxgIRremote::~CxgIRremote() {
  _resetFields();
}

void CxgIRremote::attach(uint8_t pin) {
  this->pin = pin;
  pinMode(pin, INPUT);
}

void CxgIRremote::handleRisingInterrupt() {
  //虽然是上升沿中断, 但是也要看一下电平状态, 防止误触发搞得怀疑人生, 别问我怎么知道的
  if(!digitalRead(this->pin)) {
    return;
  }
  unsigned long t = micros();
  unsigned long diffTime = t - lastRisingTime;
  //查看信号上升沿间隔,找规律, 写下面的解析代码
  // Serial.print(diffTime);
  // Serial.print(",");
  lastRisingTime = t;
  if(diffTime > 120000) {
    //清空
    IR_buf[0] = 0;
    IR_buf[1] = 0;
    IR_buf[2] = 0;
    IR_buf[3] = 0;
    return;
  }

  if(diffTime < 5200 && diffTime > 4500) {
    //引导码结束 9ms + 4.5ms
    //只看4.5ms的部分就行
    receiveIndex = 0;
    isReceiving = true;
    return;
  }
  if(diffTime > 100000 && diffTime < 120000) {
    //重复码结束 9ms + 2.25ms
    //重复码间隔110ms
    //不想要重复码的把这个注掉
    receiveIndex = 32;
    isReceiving = false;
    return;
  }

  if(diffTime < 2500 && diffTime > 2000 && isReceiving) {
    //数字1 2.25ms
    uint8_t index = receiveIndex / 8;
    IR_buf[index] |= (1 << (receiveIndex % 8));
    receiveIndex++;
  } else if(diffTime < 1400 && diffTime > 1000 && isReceiving) {
    //数字0 1.12ms
    uint8_t index = receiveIndex / 8;
    IR_buf[index] &= ~(1 << (receiveIndex % 8));
    receiveIndex++;
  }

  //接收位数,一般都是32位, 像空调遥控器100多位, 自己改改大小
  if(receiveIndex >= 32) {
    isReceiving = false;
  }
}

//检查反码进行验证, 返回去掉反码
//在循环中检查是否获取到了数据,没获取到返回0
uint16_t CxgIRremote::read() {
  if(receiveIndex == 32) {
    receiveIndex = 0;
    uint16_t data = (bitReverse(IR_buf[0]) << 8) + bitReverse(IR_buf[2]);
    uint16_t checkData = (bitReverse(~IR_buf[1]) << 8) + bitReverse(~IR_buf[3]);
    //检查反码
    if(data == checkData) {
      return data;
    }
  }
  return 0;
}

//有的遥控器没有反码,就不校验了,直接返回32位数据
//在循环中检查是否获取到了数据,没获取到返回0
uint32_t CxgIRremote::read32() {
  if(receiveIndex == 32) {
    receiveIndex = 0;
    uint32_t data = bitReverse(IR_buf[3]);
    data += (bitReverse(IR_buf[2]) << 8);
    data += (bitReverse(IR_buf[1]) << 16);
    data += (bitReverse(IR_buf[0]) << 24);
    return data;
  }
  return 0;
}

void CxgIRremote::_resetFields(void) {
  detachInterrupt(this->pin);
}
