#include "cxg_lc12s.h"

static JSTime jsTime(0);

//生成校验码
uint8_t getCheckCode(uint8_t* buf) {
  int sum = 0;
  for(int i = 0; i < 17; i++) {
    sum += buf[i];
  }
  return sum & 0xFF;
}

CxgLC12S::CxgLC12S() {
}

CxgLC12S::~CxgLC12S() {
}

//静态函数, 在loop循环中不断刷新状态
void CxgLC12S::refresh() {
  jsTime.refresh();
}

/**
 * @Description: 绑定串口,初始化参数
 * @param {HardwareSerial* serial} 硬串口引用
 * @param {uint32_t} baud 设置与lc12s的通信波特率
 * @param {uint8_t} setpin 设置引脚, 如果没有绑定设置引脚,那么也就不要这个库了
 * //因为设置模块时使用9600的波特率,正常通信时是别的波特率,需要动态改变波特率
 * @param {void (*)(HardwareSerial* serial, uint32_t baud)} changeBaud 串口更改波特率的方法, 不同的平台更改方法不一样,需要自定义实现
 * @param {uint8_t} cspin 片选使能引脚, 低电平使能
 */
void CxgLC12S::attach(HardwareSerial* serial, uint32_t baud, uint8_t setpin, void (*changeBaud)(HardwareSerial* serial, uint32_t baud), uint8_t cspin) {
  this->serial = serial;
  this->setpin = setpin;
  this->changeBaud = changeBaud;
  pinMode(setpin, OUTPUT);
  setBaud(baud);
  //cs引脚不是必须的, 值是255代表没有使能引脚
  this->cspin = cspin;
  if(cspin != 255) {
    pinMode(cspin, OUTPUT);
  }
}

void CxgLC12S::enable() {
  digitalWrite(setpin, 1);
  if(cspin != 255) {
    digitalWrite(cspin, 0);
  }
}

void CxgLC12S::disable() {
  digitalWrite(setpin, 1);
  if(cspin != 255) {
    digitalWrite(cspin, 1);
  }
}

// 串口波特率：
// 设置串口接口的传输速率，[参数]占 1 字节
// 设置范围：600 1200 2400 4800 9600 19200 38400
void CxgLC12S::setBaud(uint32_t baud) {
  uint8_t baudCode = 4;
  switch(baud) {
  case 600:
    baudCode = 0;
    break;
  case 1200:
    baudCode = 1;
    break;
  case 2400:
    baudCode = 2;
    break;
  case 4800:
    baudCode = 3;
    break;
  case 9600:
    baudCode = 4;
    break;
  case 19200:
    baudCode = 5;
    break;
  case 38400:
    baudCode = 6;
    break;
  }
  setBuf[9] = baudCode;
}

//得到通信波特率
uint32_t CxgLC12S::getBaud() {
  uint32_t baud = 0;
  switch(setBuf[9]) {
  case 0:
    baud = 600;
    break;
  case 1:
    baud = 1200;
    break;
  case 2:
    baud = 2400;
    break;
  case 3:
    baud = 4800;
    break;
  case 4:
    baud = 9600;
    break;
  case 5:
    baud = 19200;
    break;
  case 6:
    baud = 38400;
    break;
  }
  return baud;
}

// 射频功率设置：
// 设置发射功率，[参数]占 1 字节。
// 设置范围:0-14 级
void CxgLC12S::setRFPower(uint8_t power) {
  if(power > 14) {
    return;
  }
  setBuf[7] = power;
}

// RF信道 1Byte(0~127) 建议信道不要使用 16 的倍数
void CxgLC12S::setRFChannel(uint8_t channel) {
  if(channel > 127) {
    channel = 127;
  }
  setBuf[11] = channel;
}

//设置模块ID
void CxgLC12S::setSelfID(uint16_t id) {
  setBuf[2] = id & 0xFF00;
  setBuf[3] = id & 0x00FF;
}

//设置组网ID
void CxgLC12S::setMeshID(uint16_t id) {
  setBuf[4] = id & 0xFF00;
  setBuf[5] = id & 0x00FF;
}

//同步模块的设置参数
void CxgLC12S::syncParams() {
  changeBaud(serial, 9600);
  digitalWrite(setpin, 0);
  if(cspin != 255) {
    digitalWrite(cspin, 0);
  }
  isSetting = true;
  //写入指令
  jsTime.setTimeout([](int p1, void* p2) {
    CxgLC12S* lc12s = ( CxgLC12S* )p2;
    if(lc12s == NULL) {
      return;
    }
    //查询参数指令
    uint8_t buf[18] = {
      0xAA, 0x5C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x18};
    lc12s->serial->write(buf, 18);
    //参数读取需要5ms才能返回
    jsTime.setTimeout([](int p1, void* p2) {
      CxgLC12S* lc12s = ( CxgLC12S* )p2;
      if(lc12s == NULL) {
        return;
      }

      uint8_t readBuf[18] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      int i = 0;
      //写入成功会返回
      //AA 5D A6 31 00 01 00 00 00 04 00 64 00 00 00 12 00 59
      while(lc12s->serial->available()) {
        uint8_t d = lc12s->serial->read();
        // Serial.print(d, HEX);
        if(i <= 17) {
          readBuf[i] = d;
        }
        i++;
      }
      //设置成功
      if(i == 18 && readBuf[0] == 0xAA && readBuf[1] == 0x5D && readBuf[17] == getCheckCode(readBuf)) {
        for(int i = 4; i < 17; i++) {
          lc12s->setBuf[i] = readBuf[i];
        }
        unsigned long baud = lc12s->getBaud();
        if(baud > 0) {
          //更新为新的波特率
          lc12s->changeBaud(lc12s->serial, baud);
          lc12s->enable();
          lc12s->isSetting = false;
        }
      }
    },
      400, 0, lc12s);
  },
    10, 0, this);
}

//设置lc12s
void CxgLC12S::setLc12s() {
  //进入设置模式
  //一旦进入设置状态，指示灯会亮，SET 引脚配置必须是低电平，CS 引脚必须接低电平，
  //且串口设置必须是数据位 8，波特率 9600，校验位 N，停止位 1，空中速率 1Mbps。
  changeBaud(serial, 9600);
  digitalWrite(setpin, 0);
  if(cspin != 255) {
    digitalWrite(cspin, 0);
  }
  setBuf[17] = getCheckCode(setBuf);
  isSetting = true;

  //写入指令
  jsTime.setTimeout([](int p1, void* p2) {
    CxgLC12S* lc12s = ( CxgLC12S* )p2;
    if(lc12s == NULL) {
      return;
    }
    lc12s->serial->write(lc12s->setBuf, 18);
    //参数写入需要330ms才能返回
    jsTime.setTimeout([](int p1, void* p2) {
      CxgLC12S* lc12s = ( CxgLC12S* )p2;
      if(lc12s == NULL) {
        return;
      }
      uint8_t readBuf[18] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      int i = 0;
      //写入成功会返回 AA 5B A6 31 00 01 00 00 00 04 00 64 00 00 00 12 00 57
      while(lc12s->serial->available()) {
        uint8_t d = lc12s->serial->read();
        // Serial.write(d);
        if(i <= 17) {
          readBuf[i] = d;
        }
        i++;
      }
      //设置成功
      if(i == 18 && readBuf[0] == 0xAA && readBuf[1] == 0x5B && readBuf[17] == getCheckCode(readBuf)) {
        //更新为新的波特率
        unsigned long baud = lc12s->getBaud();
        if(baud > 0) {
          //更新为新的波特率
          lc12s->changeBaud(lc12s->serial, baud);
          lc12s->enable();
          lc12s->isSetting = false;
        }
      }
    },
      400, 0, lc12s);
  },
    10, 0, this);
}

//得到版本
void CxgLC12S::getVersion() {
  changeBaud(serial, 9600);
  digitalWrite(setpin, 0);
  if(cspin != 255) {
    digitalWrite(cspin, 0);
  }
  isSetting = true;
  //写入指令
  jsTime.setTimeout([](int p1, void* p2) {
    CxgLC12S* lc12s = ( CxgLC12S* )p2;
    if(lc12s == NULL) {
      return;
    }
    //查询版本号指令
    uint8_t buf[18] = {
      0xAA, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07};
    lc12s->serial->write(buf, 18);
    //参数读取需要5ms才能返回
    jsTime.setTimeout([](int p1, void* p2) {
      CxgLC12S* lc12s = ( CxgLC12S* )p2;
      if(lc12s == NULL) {
        return;
      }

      uint8_t readBuf[3] = {0x00, 0x00, 0x00};
      int i = 0;
      //写入成功会返回 02 00 05（HEX 格式）
      while(lc12s->serial->available()) {
        uint8_t d = lc12s->serial->read();
        if(i <= 2) {
          readBuf[i] = d;
        }
        i++;
      }
      //设置成功
      if(i == 3) {
        //通过默认串口打印出来
        Serial.println();
        Serial.print("LC12S version: ");
        Serial.print(readBuf[0]);
        Serial.print(".");
        Serial.print(readBuf[1]);
        Serial.println(readBuf[2]);

        unsigned long baud = lc12s->getBaud();
        if(baud > 0) {
          //更新为新的波特率
          lc12s->changeBaud(lc12s->serial, baud);
          lc12s->enable();
          lc12s->isSetting = false;
        }
      }
    },
      400, 0, lc12s);
  },
    10, 0, this);
}
