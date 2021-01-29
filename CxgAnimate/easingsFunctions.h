//缓动函数库
//取自https://github.com/ai/easings.net/blob/master/src/easings/easingsFunctions.ts
//这些缓动函数都是直角坐标系, 运行时间与位移的函数图
//x轴为运行时间所占总时间的比例, 取值范围[0-1]
//返回的值是位移的变化比例, 范围也是[0-1]
#include "Arduino.h"
#ifndef easingsFunctions_H_
#define easingsFunctions_H_

class Easings {
  public:
  Easings();
  static float linear(float x);
  static float easeInQuad(float x);
  static float easeOutQuad(float x);
  static float easeInOutQuad(float x);
  static float easeInCubic(float x);
  static float easeOutCubic(float x);
  static float easeInOutCubic(float x);
  static float easeInQuart(float x);
  static float easeOutQuart(float x);
  static float easeInOutQuart(float x);
  static float easeInQuint(float x);
  static float easeOutQuint(float x);
  static float easeInOutQuint(float x);
  static float easeInSine(float x);
  static float easeOutSine(float x);
  static float easeInOutSine(float x);
  static float easeInExpo(float x);
  static float easeOutExpo(float x);
  static float easeInOutExpo(float x);
  static float easeInCirc(float x);
  static float easeOutCirc(float x);
  static float easeInOutCirc(float x);
  static float easeInBack(float x);
  static float easeOutBack(float x);
  static float easeInOutBack(float x);
  static float easeInElastic(float x);
  static float easeOutElastic(float x);
  static float easeInOutElastic(float x);
  static float easeInBounce(float x);
  static float easeOutBounce(float x);
  static float easeInOutBounce(float x);
};

#endif
