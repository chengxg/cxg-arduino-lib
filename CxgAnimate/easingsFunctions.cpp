//缓动函数库
//取自https://github.com/ai/easings.net/blob/master/src/easings/easingsFunctions.ts
//这些缓动函数都是直角坐标系中运行时间与位移的函数图
//x轴为运行时间所占总时间的比例, 取值范围[0-1]
//返回的值是位移的变化比例, 范围也是[0-1]
//函数曲线见:https://easings.net/ https://www.xuanfengge.com/easeing/easeing/
#include "easingsFunctions.h"
#define c1 1.70158
#define c2 2.59491   // c1 * 1.525
#define c3 2.70158   // c1 + 1
#define c4 2.094395  //(2 * PI) / 3
#define c5 1.396263  //(2 * PI) / 4.5

#define n1 7.5625
#define d1 2.75

//基础方法
float bounceOut(float x) {
  // const n1 = 7.5625;
  // const d1 = 2.75;

  // if(x < 1 / d1) {
  //   return n1 * x * x;
  // } else if(x < 2 / d1) {
  //   return n1 * (x -= 1.5 / d1) * x + 0.75;
  // } else if(x < 2.5 / d1) {
  //   return n1 * (x -= 2.25 / d1) * x + 0.9375;
  // } else {
  //   return n1 * (x -= 2.625 / d1) * x + 0.984375;
  // }

  if(x < 0.363636) {
    return n1 * x * x;
  } else if(x < 0.727273) {
    return n1 * (x -= 0.545455) * x + 0.75;
  } else if(x < 0.909091) {
    return n1 * (x -= 0.818182) * x + 0.9375;
  } else {
    return n1 * (x -= 0.954545) * x + 0.984375;
  }
}

Easings::Easings() {
}

float Easings::linear(float x) {
  return x;
}

float Easings::easeInQuad(float x) {
  return x * x;
}
float Easings::easeOutQuad(float x) {
  // return 1 - (1 - x) * (1 - x);
  return 1.0 - (1.0 - x) * (1.0 - x);
}
float Easings::easeInOutQuad(float x) {
  // return x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
  if(x < 0.5) {
    return 2.0 * x * x;
  }
  float temp = -2.0 * x + 2.0;
  return 1.0 - temp * temp / 2.0;
}
float Easings::easeInCubic(float x) {
  return x * x * x;
}
float Easings::easeOutCubic(float x) {
  // return 1.0 - pow(1.0 - x, 3);
  float temp = 1.0 - x;
  return 1.0 - temp * temp * temp;
}
float Easings::easeInOutCubic(float x) {
  // return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
  if(x < 0.5) {
    return 4.0 * x * x * x;
  }
  float temp = -2.0 * x + 2.0;
  return 1 - temp * temp * temp / 2.0;
}
float Easings::easeInQuart(float x) {
  return x * x * x * x;
}
float Easings::easeOutQuart(float x) {
  // return 1 - pow(1 - x, 4);
  float temp = 1.0 - x;
  return 1.0 - temp * temp * temp * temp;
}
float Easings::easeInOutQuart(float x) {
  // return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2;
  if(x < 0.5) {
    return 8.0 * x * x * x * x;
  }
  float temp = -2.0 * x + 2.0;
  return 1 - temp * temp * temp * temp / 2;
}
float Easings::easeInQuint(float x) {
  return x * x * x * x * x;
}
float Easings::easeOutQuint(float x) {
  // return 1 - pow(1 - x, 5);
  float temp = 1.0 - x;
  return 1.0 - temp * temp * temp * temp * temp;
}
float Easings::easeInOutQuint(float x) {
  // return x < 0.5 ? 16 * x * x * x * x * x : 1 - pow(-2 * x + 2, 5) / 2;
  if(x < 0.5) {
    return 16.0 * x * x * x * x * x;
  }
  float temp = -2.0 * x + 2.0;
  return 1.0 - temp * temp * temp * temp * temp / 2.0;
}
float Easings::easeInSine(float x) {
  return 1.0 - cos((x * PI) / 2.0);
}
float Easings::easeOutSine(float x) {
  return sin((x * PI) / 2.0);
}
float Easings::easeInOutSine(float x) {
  return -(cos(PI * x) - 1.0) / 2.0;
}
float Easings::easeInExpo(float x) {
  // return x == 0 ? 0 : pow(2, 10 * x - 10);
  if(abs(x) < 0.001) {
    return 0;
  }
  return pow(2.0, 10.0 * x - 10.0);
}
float Easings::easeOutExpo(float x) {
  // return x == 1 ? 1 : 1 - pow(2, -10 * x);
  if(abs(x - 1.0) < 0.001) {
    return 1;
  }
  return 1.0 - pow(2, -10.0 * x);
}
float Easings::easeInOutExpo(float x) {
  // return x == 0
  //          ? 0
  //          : x == 1
  //              ? 1
  //              : x < 0.5
  //                  ? pow(2, 20 * x - 10) / 2
  //                  : (2 - pow(2, -20 * x + 10)) / 2;
  if(abs(x) < 0.001) {
    return 0;
  }
  if(abs(x - 1.0) < 0.001) {
    return 1;
  }
  if(x < 0.5) {
    return pow(2.0, 20.0 * x - 10.0) / 2.0;
  }
  return (2.0 - pow(2.0, -20.0 * x + 10.0)) / 2.0;
}
float Easings::easeInCirc(float x) {
  // return 1 - sqrt(1 - pow(x, 2));
  return 1.0 - sqrt(1.0 - x * x);
}
float Easings::easeOutCirc(float x) {
  // return sqrt(1 - pow(x - 1, 2));
  float temp = x - 1.0;
  return sqrt(1.0 - temp * temp);
}
float Easings::easeInOutCirc(float x) {
  // return x < 0.5
  //          ? (1 - sqrt(1 - pow(2 * x, 2))) / 2
  //          : (sqrt(1 - pow(-2 * x + 2, 2)) + 1) / 2;
  if(x < 0.5) {
    return (1.0 - sqrt(1.0 - 4.0 * x * x)) / 2.0;
  }
  float temp = -2.0 * x + 2.0;
  return (sqrt(1.0 - temp * temp) + 1.0) / 2.0;
}
float Easings::easeInBack(float x) {
  return c3 * x * x * x - c1 * x * x;
}
float Easings::easeOutBack(float x) {
  // return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);
  float temp = x - 1.0;
  return 1.0 + c3 * temp * temp * temp + c1 * temp * temp;
}
float Easings::easeInOutBack(float x) {
  // return x < 0.5
  //          ? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
  //          : (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
  if(x < 0.5) {
    return (4.0 * x * x * ((c2 + 1.0) * 2.0 * x - c2)) / 2.0;
  }
  float temp = 2.0 * x - 2.0;
  return (temp * temp * ((c2 + 1.0) * temp + c2) + 2.0) / 2.0;
}
float Easings::easeInElastic(float x) {
  // return x == 0
  //          ? 0
  //          : x == 1
  //              ? 1
  //              : -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4);
  if(abs(x) < 0.0001) {
    return 0;
  }
  if(abs(x - 1.0) < 0.0001) {
    return 1;
  }
  return -pow(2.0, 10.0 * x - 10.0) * sin((x * 10.0 - 10.75) * c4);
}
float Easings::easeOutElastic(float x) {
  // return x == 0
  //          ? 0
  //          : x == 1
  //              ? 1
  //              : pow(2, -10 * x) * sin((x * 10 - 0.75) * c4) + 1;
  if(abs(x) < 0.0001) {
    return 0;
  }
  if(abs(x - 1.0) < 0.0001) {
    return 1;
  }
  return pow(2.0, -10.0 * x) * sin((x * 10.0 - 0.75) * c4) + 1.0;
}
float Easings::easeInOutElastic(float x) {
  // return x == 0
  //          ? 0
  //          : x == 1
  //              ? 1
  //              : x < 0.5
  //                  ? -(pow(2, 20 * x - 10) * sin((20 * x - 11.125) * c5)) / 2
  //                  : (pow(2, -20 * x + 10) * sin((20 * x - 11.125) * c5)) / 2 + 1;
  if(abs(x) < 0.0001) {
    return 0;
  }
  if(abs(x - 1.0) < 0.0001) {
    return 1;
  }
  if(x < 0.5) {
    return -(pow(2.0, 20.0 * x - 10.0) * sin((20.0 * x - 11.125) * c5)) / 2.0;
  }
  return (pow(2.0, -20.0 * x + 10.0) * sin((20.0 * x - 11.125) * c5)) / 2.0 + 1.0;
}
float Easings::easeInBounce(float x) {
  return 1.0 - bounceOut(1.0 - x);
}
float Easings::easeOutBounce(float x) {
  return bounceOut(x);
}
float Easings::easeInOutBounce(float x) {
  return x < 0.5
           ? (1 - bounceOut(1 - 2.0 * x)) / 2.0
           : (1 + bounceOut(2.0 * x - 1)) / 2.0;
}
