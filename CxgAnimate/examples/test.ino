#include <Arduino.h>
#include "cxg_JSTime.h"
#include "cxg_Animate.h"

//支持arduino uno, stm32, esp32
//测试于arduino uno
JSTime jsTime;
CxgAnimate animate;

int animateId = 0;

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
      Serial.print("animate end");
      return true;
    },
    //动画持续时间
    1500,
    //动画循环次数, 设置0为无限循环, 往返需要两次才能看出效果
    2,
    //动画执行方向 1:正向执行,0:往返执行, -1:反向执行
    0);

  jsTime.setTimeout([]() {
    //延时一段时间运行动画
    //播放动画
    //第一个参数,动画id
    //第二个参数,动画帧间隔时间, 越短越细腻越费资源
    animate.play(animateId, 20);
  },
    1000);
}

void loop() {
  jsTime.refresh();
  animate.refresh();
}
