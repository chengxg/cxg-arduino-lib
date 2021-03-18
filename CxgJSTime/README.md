Arduino JSTime Library
===

该Arduino Library实现了非阻塞延时执行功能, 像在javascript中使用setTimeout 和 setInterval那样方便。
The Arduino library implements non blocking delay execution function, which is as convenient as using setTimeout and setinterval in JavaScript.

## 快速开始
## Quick start

将该Library复制到您的Arduino/Libraries下。
Copy the library to your Arduino / libraries.

```CPP
#include <Arduino.h>
#include "cxg_JSTime.h"
```

### 初始化一个JSTime 
### Initialize a jstime

```CPP
Serial.begin(115200);
/**
 * 初始化一个jsTime容器 
 * 有一个参数是初始化的定时器个数, 默认值为1
 * 
 * Initialize a jstime container
 * There is a parameter to initialize the number of timers, the default value is 1
 */
JSTime jsTime;
// JSTime jsTime(1);
```

### 添加延时执行setTimeout 
### Add delay execution setTimeout

```CPP
 //回调函数延时1s执行, 执行完毕后立即销毁, 代替delay函数使用
 //返回timeId 是正数, clearTime时使用
 //The callback function is executed with a delay of 1 s, and is destroyed immediately after execution, instead of delay function
 //The return timeid is a positive number, which is used when cleartime
 int timeId = jsTime.setTimeout([]() {
    Serial.println("setTimeout 1000ms");
  },
    1000);
```

### 添加延时间隔执行setInterval 
### Add delay interval to execute setinterval

```CPP
 //回调函数每隔1s执行一次，不会自动停止
 //返回timeId 是负数, clearTime时使用
 //The callback function is executed every 1s and does not stop automatically
 //The return timeid is a negative number, which is used when cleartime
 int timeId = jsTime.setInterval([]() {
    Serial.println("setInterval 1000ms");
  },
    1000);
```

### 取消定时器执行clearTime
### Cancel timer to execute cleartime

```CPP
  //传入timeId 取消该定时器执行
  //如果timeId为0或不传入, 则全部取消
  //Pass in timeid to cancel the timer execution
  //Cancel all if timeid is 0 or not passed in
  jsTime.clearTime(timeId);

  //全部取消
  //cancel all
  jsTime.clearTime();
```

### 需要在loop中不断刷新jsTime
### You need to refresh jstime in the loop

```CPP
void loop() {
  jsTime.refresh();
  // Do other things...
}
```


## 允许嵌套执行
## Allow nested execution

```CPP
  //无阻塞延时2s
  //Non blocking delay 2 s
  jsTime.setTimeout([]() {
    Serial.println("setTimeout delay 2000ms");
    //延时嵌套
    //Delay nesting
    jsTime.setTimeout([]() {
      Serial.println("nesting setTimeout delay 1000ms");
    },
      1000);
  },
    2000);

```
   