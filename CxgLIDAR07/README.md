# CXG_LIDAR07

LIDAR07-100W 测距模组是一个基于单点TOF技术，采用850nm LED光源，配合独特的光学、结构、电路设计而成的低成本测距模组，实现中短距离的测距需求，测距范围为 0.2m～12m，结合相应的滤波算法处理，可以得到极低的测量噪声。

LIDAR07-100W测距模组搭载了850nm 的窄带滤波片，可以有效滤除99%的环境光，保证在不同环境下测距的准确性，兼顾室内/室外的应用场景。LIDAR07-100W 测距模组提供多种通讯接口，同时支持 IIC 和 UART 的通讯，
便于终端产品的集成。在测距方面提供多种测量方式，单次自动曝光测量以及连续自动曝光测量，以满足不同终端产品的实际使用需求。在数据输出方面，可以选择使用滤波，从而得到更加平滑的距离数据；或者不使
用滤波，从而得到更快的动态测距响应。在非测量期间，模块不再进行测试，从而有效的降低功耗，以及保证光源的使用寿命。

LIDAR07-100W 测距模块在出厂前经过多道矫正标定和测试，具有很好的一致性。

本项目参考自[https://wiki.dfrobot.com.cn/_SKU_SEN0413_TOF_IR_Distance_Sensor_0.2_12m](https://wiki.dfrobot.com.cn/_SKU_SEN0413_TOF_IR_Distance_Sensor_0.2_12m)

## 使用串口

```C++
static CxgLIDAR07 LIDAR07;
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  LIDAR07.attachUart(&Serial1);

  LIDAR07.setMeasureDistanceCallback([](uint16_t distance, uint16_t temperature, uint16_t amplitude, uint16_t backgroundLight) {
    if(distance > 0) {
      Serial.print("distance: ");
      Serial.print(distance);
      Serial.print(" temperature: ");
      Serial.print(temperature);
      Serial.print(" amplitude: ");
      Serial.print(amplitude);
      Serial.print(" background: ");
      Serial.print(backgroundLight);
      Serial.println();
    }
  });
}

void loop() {
  LIDAR07.refresh();
}

```

## 使用 IIC

```C++
static CxgLIDAR07 LIDAR07;
void setup() {
  Serial.begin(115200);
  Wire1.begin(sda_pin, scl_pin, 400000);
  LIDAR07.attachWire(&Wire1, rx_pin, 0x70);

  LIDAR07.setMeasureDistanceCallback([](uint16_t distance, uint16_t temperature, uint16_t amplitude, uint16_t backgroundLight) {
    if(distance > 0) {
      Serial.print("distance: ");
      Serial.print(distance);
      Serial.print(" temperature: ");
      Serial.print(temperature);
      Serial.print(" amplitude: ");
      Serial.print(amplitude);
      Serial.print(" background: ");
      Serial.print(backgroundLight);
      Serial.println();
    }
  });
}

void loop() {
  LIDAR07.refresh();
}

```

## History

- Data 2022-1-02
- Version V1.0.0


