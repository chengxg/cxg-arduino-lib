#include <Arduino.h>
#include "cxg_JSTime.h"
#include "cxg_Animate.h"

//支持arduino uno, stm32, esp32
//测试于arduino uno
JSTime jsTime;
CxgAnimate animate;

//返回一个函数指针的函数
//获取缓动函数
float (*getEasingFunc(int i))(float x) {
  switch(i) {
  case 1:
    return Easings::linear;
    break;
  case 2:
    return Easings::easeInQuad;
    break;
  case 3:
    return Easings::easeOutQuad;
    break;
  case 4:
    return Easings::easeInOutQuad;
    break;
  case 5:
    return Easings::easeInCubic;
    break;
  case 6:
    return Easings::easeOutCubic;
  case 7:
    return Easings::easeInOutCubic;
  case 8:
    return Easings::easeInQuart;
  case 9:
    return Easings::easeOutQuart;
  case 10:
    return Easings::easeInOutQuart;
  case 11:
    return Easings::easeInQuint;
  case 12:
    return Easings::easeOutQuint;
  case 13:
    return Easings::easeInOutQuint;
  case 14:
    return Easings::easeInSine;
  case 15:
    return Easings::easeOutSine;
  case 16:
    return Easings::easeInOutSine;
  case 17:
    return Easings::easeInExpo;
  case 18:
    return Easings::easeOutExpo;
  case 19:
    return Easings::easeInOutExpo;
  case 20:
    return Easings::easeInCirc;
  case 21:
    return Easings::easeOutCirc;
  case 22:
    return Easings::easeInOutCirc;
  case 23:
    return Easings::easeInBack;
  case 24:
    return Easings::easeOutBack;
  case 25:
    return Easings::easeInOutBack;
  case 26:
    return Easings::easeInElastic;
  case 27:
    return Easings::easeOutElastic;
  case 28:
    return Easings::easeInOutElastic;
  case 29:
    return Easings::easeInBounce;
  case 30:
    return Easings::easeOutBounce;
  case 31:
    return Easings::easeInOutBounce;
    break;
  }
  return NULL;
}

int animateId = 0;
int easingId = 0;

void setup() {
  Serial.begin(115200);
  delay(3000);

  //创建一个动画,返回动画的id,如果返回id是0则说明有问题
  //创建成功后不会立即执行,需要调用animate.play或animate.restart来播放动画
  animateId = animate.create(
    //按照动画运行时间比例来执行缓动的动作函数
    [](float x) {
      //计算公式 y=a+(b-a)*x;
      //a:初始值
      //b:终止值
      //x:变化比例
      //y:实际值

      //例如:
      //按照一定规律输出 100HZ-1200HZ 的频率
      // tone(3, 100 + (1200 - 100) * x, 200);
      Serial.print(x);
      Serial.print(",");
    },
    //缓动函数, 内置了一些缓动函数都在Easings下 函数曲线图见: https://easings.net/ https://www.xuanfengge.com/easeing/easeing/
    //横坐标: 动画运行时间比例, 取值范围[0-1] 纵坐标: 返回目标改变比例, 取值范围[0-1]
    Easings::linear,
    //动画执行完毕后的回调, 返回false,该动画一直会保留到内存中,不释放, 返回true或未设置该回调,则释放内存
    [](int id) -> bool {
      easingId++;
      Serial.println("");
      Serial.print("next animate easings: ");
      Serial.println(easingId);
      CxgAnimateStruct* item = animate.getAnimate(id);
      if(item != NULL) {
        if(getEasingFunc(easingId)) {
          item->easingFunc = getEasingFunc(easingId);
          jsTime.setTimeout([]() {
            animate.restart(animateId, 20);
          },
            1000);
        } else {
          //销毁动画
          return true;
        }
      }
      //返回 false,保留该动画不被销毁
      return false;
    },
    //动画持续时间
    1500,
    //动画循环次数, 设置0为无限循环, 往返需要两次才能看出效果
    2,
    //动画执行方向 1:正向执行,0:往返执行, -1:反向执行
    0);

  animate.setKeyFrameMode(animateId, false);
}

void loop() {
  jsTime.refresh();
  animate.refresh();

  // delay(25);//循环模拟超时

  while(Serial.available()) {
    char a = Serial.read();
    if(a == 'a') {
      //延时一段时间运行动画
      animate.restart(animateId, 20);
    }
    if(a == 'b') {
      //延时一段时间运行动画
      animate.play(animateId, 20, true);
    }
    if(a == 'c') {
      //延时一段时间运行动画
      animate.pause(animateId);
    }
  }
}
