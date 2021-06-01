/*
 * 借鉴自web前端的动画库, 实现的arduino动画库
 * @Author: chengxg
 * @Date: 2021-01-24
 */
#ifndef CXG_Animate_H_
#define CXG_Animate_H_

#include "Arduino.h"
#include "easingsFunctions.h"

struct CxgAnimateStruct {
  unsigned long lastTickTime = 0;       //上次tick执行时间
  int id = 0;                           //分配的id,一定不会重复
  uint32_t startCount = 0;              //从play开始进行动画tick计数, isKeyFrameMode=true时, 累计帧数, isKeyFrameMode=false时,累计运行时间
  uint16_t durationTime = 0;            //一次动画持续时间
  uint16_t frameIntevalTime = 0;        //动画执行帧时间间隔
  uint16_t loopCount = 1;               //重复执行次数, 0为无限循环
  int8_t direction = 1;                 //动画执行方向 1:正向执行,0:往返执行, -1:反向执行
  bool isSuspend = true;                //是否暂停运行, true:暂停, false:执行
  bool isKeyFrameMode = true;           //是否是帧模式 true:帧计数, false:时间计数
  float (*easingFunc)(float x) = NULL;  //缓动函数, 变化时间比例与改变比例的函数公式 取值范围[0-1]
  void (*setChange)(float x) = NULL;    //改变函数, 传入改变比例,取值范围[0-1]
  bool (*endCallback)(int id) = NULL;   //动画执行完成回调, 返回true,立即销毁, 否则一直保留在内存中
};

class CxgAnimate {
  private:
  int arrLen = 0;
  int num = 0;

  struct CxgAnimateStruct** arr = NULL;
  bool setSize();
  bool tick(struct CxgAnimateStruct* animate, unsigned long t);

  public:
  CxgAnimate(int intSize = 1);
  ~CxgAnimate();

  struct CxgAnimateStruct* getAnimate(int id);
  int create(void (*setChange)(float x), float (*easingFunc)(float x), bool (*endCallback)(int id), uint16_t durationTime, uint16_t loopCount = 1, int8_t direction = 1);
  void refresh();
  bool tick(int id);

  bool restart(int id, uint16_t frameIntevalTime = 17);
  bool play(int id, uint16_t frameIntevalTime = 17, bool includePauseTime = false);
  bool pause(int id);

  bool setChangeFunc(int id, void (*endCallback)(float x));
  bool setEasingFunc(int id, float (*easingFunc)(float x));
  bool setEndCallback(int id, bool (*endCallback)(int id));

  bool setDurationTime(int id, uint16_t durationTime);
  bool setLoopCount(int id, uint16_t loopCount);
  bool setDirection(int id, int8_t direction);
  bool setKeyFrameMode(int id, bool isKeyFrameMode);

  bool clear(int id);

  //获取活动的动画总数
  int getNumber();
  //获取所有动画结构体占用的内存大小
  int getHeapMemorySize();
};

#endif