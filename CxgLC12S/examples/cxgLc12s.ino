#include <Arduino.h>

//依赖我的JSTime库
#include "cxg_JSTime.h"
#include "cxg_lc12s.h"

//该库支持esp32, stm32, arduino uno等
//注意该模块仅支持3.3v,5v肯定会烧毁, 用在arduino uno上需要电平转换
//该示例以esp32为开发环境

//lc12s引脚
#define lc12s_rx_pin 26
#define lc12s_tx_pin 25
#define lc12s_set_pin 33
#define lc12s_cs_pin 32  //如果不需要绑定cs引脚则设置为255

static JSTime jsTime;
static CxgLC12S lc12s;

void setup() {
  Serial.begin(115200);

  //esp32可以将串口引脚映射到别的引脚上
  Serial1.begin(38400, SERIAL_8N1, lc12s_tx_pin, lc12s_rx_pin);
  //stm32f103c8t6串口1或其他串口（TX=PA9,RX=PA10）, arduino uno硬件串口1 硬串口（TX–1,RX–0）
  //Serial1.begin(38400);
  lc12s.attach(
    &Serial1, 38400, lc12s_set_pin, [](HardwareSerial* serial, uint32_t baud) {
      //esp32直接可以更改波特率
      serial->updateBaudRate(baud);
      //stm32,arduino uno需要重新调用begin来更改波特率
      // serial->begin(baud);
    },
    lc12s_cs_pin);

  lc12s.enable();
  //设置信道,用来避免多个模块干扰的
  lc12s.setRFChannel(10);
  //设置自身id
  lc12s.setSelfID(0x0001);
  //设置组网id
  lc12s.setMeshID(0x0001);
  //信道,自身id,组网id都相同才可以通信

  jsTime.setTimeout([]() {
    //上面设置的参数需要调用这个方法才能完成设置
    //参数设置完成模块自动保存,下次开机就不用再次设置了
    lc12s.setLc12s();
    //设置完一段时间才能正常通信

    //获取模块的版本, 会通过串口打印出来
    // lc12s.getVersion();

    //获取当前模块的设置参数, 获取后的数据会设置到 lc12s.setBuf数组中
    // lc12s.syncParams();

    //由于模块写入指令需要延时330ms才能返回, 以上这三个方法500ms内只能调用其中一个
  },
    2000);
}

void loop() {
  jsTime.refresh();
  //不断刷新模块
  CxgLC12S::refresh();

  //在模块设置的过程中不能读取数据
  if(!lc12s.isSetting) {
    //读取收到的串口数据
    while(Serial1.available()) {
      Serial.write(Serial1.read());
    }
  }
}
