#include "cxg_Animate.h"

static int zero = 0;
static int maxId = ~zero / 2 - 2;
static int createId = 0;

CxgAnimate::CxgAnimate(int intSize) {
  if(intSize > 10) {
    intSize = 10;
  }
  if(intSize < 0) {
    intSize = 0;
  }
  arrLen = intSize;
  arr = ( struct CxgAnimateStruct** )malloc(sizeof(struct CxgAnimateStruct*) * arrLen);
  if(arr != NULL) {
    for(int i = 0; i < arrLen; i++) {
      arr[i] = NULL;
    }
  }
}

CxgAnimate::~CxgAnimate() {
  clear(0);
}

bool CxgAnimate::setSize() {
  //扩容
  if(num > arrLen) {
    int newArrLen = arrLen + arrLen * 0.5 + 1;
    struct CxgAnimateStruct** newArr = ( struct CxgAnimateStruct** )malloc(
      sizeof(struct CxgAnimateStruct*) * newArrLen);
    if(newArr == NULL) {
      //申请内存失败
      return false;
    }
    for(int i = 0; i < newArrLen; i++) {
      if(i < arrLen) {
        newArr[i] = arr[i];
      } else {
        newArr[i] = NULL;
      }
    }
    if(arr != NULL) {
      free(arr);
    }
    arr = newArr;
    arrLen = newArrLen;
  }

  //缩容
  if((arrLen / 2 >= 4) && num <= arrLen / 4) {
    int newArrLen = arrLen / 2;
    struct CxgAnimateStruct** newArr = ( struct CxgAnimateStruct** )malloc(
      sizeof(struct CxgAnimateStruct*) * newArrLen);
    if(newArr == NULL) {
      //申请内存失败
      return false;
    }
    for(int i = 0; i < newArrLen; i++) {
      newArr[i] = NULL;
    }
    int j = 0;
    for(int i = 0; i < arrLen; i++) {
      if(arr[i] != NULL) {
        newArr[j] = arr[i];
        j++;
      }
    }
    if(arr != NULL) {
      free(arr);
    }
    arr = newArr;
    arrLen = newArrLen;
  }
  return true;
}

int CxgAnimate::create(void (*setChange)(float x), float (*easingFunc)(float x), bool (*endCallback)(int id), uint16_t durationTime, uint16_t loopCount, int8_t direction) {
  if(durationTime == 0) {
    return 0;
  }
  struct CxgAnimateStruct* animate = ( CxgAnimateStruct* )malloc(sizeof(CxgAnimateStruct));
  animate->setChange = setChange;
  animate->durationTime = durationTime;
  if(easingFunc == NULL) {
    easingFunc = Easings::linear;
  }
  animate->easingFunc = easingFunc;
  if(direction > 1) {
    direction = 1;
  }
  if(direction < -1) {
    direction = -1;
  }
  animate->direction = direction;
  animate->loopCount = loopCount;
  animate->endCallback = endCallback;
  animate->isSuspend = true;
  animate->isKeyFrameMode = true;
  animate->startCount = 0;
  if(createId > maxId) {
    createId = 1000;
  }
  createId++;
  num++;
  animate->id = createId;
  bool isSuccess = setSize();
  if(!isSuccess) {
    free(animate);
    return 0;
  }

  //防止id重复
  bool isDuplicateId = false;
  while(true) {
    for(int i = 0; i < arrLen; i++) {
      if(arr[i] != NULL && arr[i]->id == createId) {
        isDuplicateId = true;
        break;
      }
    }
    if(isDuplicateId) {
      createId++;
      if(createId > maxId) {
        createId = 1000;
      }
      animate->id = createId;
      isDuplicateId = false;
    } else {
      break;
    }
  }

  for(int i = 0; i < arrLen; i++) {
    if(arr[i] == NULL) {
      arr[i] = animate;
      return animate->id;
    }
  }

  free(animate);
  return 0;
}

struct CxgAnimateStruct* CxgAnimate::getAnimate(int id) {
  for(int i = 0; i < arrLen; i++) {
    if(arr[i] != NULL && arr[i]->id == id) {
      return arr[i];
    }
  }
  return NULL;
}

bool CxgAnimate::tick(struct CxgAnimateStruct* animate, unsigned long t) {
  if(animate == NULL) {
    return false;
  }
  if(animate->isSuspend) {
    return false;
  }
  unsigned long diffTime = t - animate->lastTickTime;
  if(diffTime < animate->frameIntevalTime) {
    return false;
  }

  animate->lastTickTime = t;
  if(animate->frameIntevalTime == 0) {
    return false;
  }
  if(animate->durationTime == 0) {
    return false;
  }

  //帧优先,必须执行完frameNum才能完成动画
  if(animate->isKeyFrameMode) {
    animate->startCount++;
  } else {
    //时间优先, 到动画运行时间结束完成动画
    animate->startCount += diffTime;
  }

  //都转化为帧数
  uint32_t exeFrameCount = animate->startCount;
  if(!animate->isKeyFrameMode) {
    exeFrameCount = exeFrameCount / animate->frameIntevalTime;
  }

  uint16_t nextDelayFrameNum = 5;                                         //下周期循环延时帧数
  uint16_t frameNum = animate->durationTime / animate->frameIntevalTime;  //帧数
  uint16_t allFrameNum = frameNum + 1 + nextDelayFrameNum;                //完整执行一次所需的帧数

  //本次动画已过帧
  uint16_t passFrameNum = exeFrameCount % allFrameNum;
  if(passFrameNum == 0) {
    passFrameNum = allFrameNum - 1;
  } else {
    passFrameNum--;
  }
  bool isFinite = (animate->loopCount > 0);               //是否执行有限次
  uint16_t exeCount = (exeFrameCount - 1) / allFrameNum;  //计算执行总次数

  if(passFrameNum > frameNum) {
    exeCount++;
  }

  //执行完成
  if(isFinite && exeCount >= animate->loopCount) {
    animate->isSuspend = true;  //暂停运行
    animate->startCount = 0;    //计数清0

    if(animate->endCallback != NULL) {
      int id = animate->id;
      bool isClear = animate->endCallback(id);
      //是否清除
      if(isClear) {
        return clear(id);
      }
      return true;
    }
    return clear(animate->id);
  }

  if(passFrameNum > frameNum) {
    return false;
  }

  float y = 0.0;
  float pt = 0.0;
  //正向执行
  if(animate->direction == 1
     || (animate->direction == 0 && exeCount % 2 == 0)) {
    pt = ( float )passFrameNum / frameNum;
  }
  //反向执行
  if(animate->direction == -1
     || (animate->direction == 0 && exeCount % 2 == 1)) {
    pt = 1.0 - (( float )passFrameNum / frameNum);
  }

  if(animate->easingFunc != NULL) {
    y = animate->easingFunc(pt);
  }
  if(animate->setChange != NULL) {
    animate->setChange(y);
  }
  return true;
}

void CxgAnimate::refresh() {
  unsigned long t = millis();
  for(int i = 0; i < arrLen; i++) {
    if(arr == NULL) {
      return;
    }
    if(arr[i] == NULL) {
      continue;
    }
    struct CxgAnimateStruct* animate = arr[i];
    tick(animate, t);
  }
}

bool CxgAnimate::tick(int id) {
  struct CxgAnimateStruct* animate = getAnimate(id);
  return tick(animate, millis());
}

bool CxgAnimate::restart(int id, uint16_t frameIntevalTime) {
  struct CxgAnimateStruct* animate = getAnimate(id);
  if(!animate) {
    return false;
  }
  if(frameIntevalTime == 0) {
    return false;
  }
  animate->lastTickTime = millis();
  animate->startCount = 0;
  animate->isSuspend = false;
  animate->frameIntevalTime = frameIntevalTime;
  return tick(animate, millis());
}

bool CxgAnimate::play(int id, uint16_t frameIntevalTime, bool includePauseTime) {
  struct CxgAnimateStruct* animate = getAnimate(id);
  if(!animate) {
    return false;
  }
  if(frameIntevalTime == 0) {
    return false;
  }
  animate->isSuspend = false;
  animate->frameIntevalTime = frameIntevalTime;
  if(!includePauseTime) {
    animate->lastTickTime = millis();
  }
  return tick(animate, millis());
}

bool CxgAnimate::pause(int id) {
  struct CxgAnimateStruct* animate = getAnimate(id);
  if(!animate) {
    return false;
  }
  animate->isSuspend = true;
  return true;
}

bool CxgAnimate::setChangeFunc(int id, void (*setChange)(float x)) {
  struct CxgAnimateStruct* animate = getAnimate(id);
  if(!animate) {
    return false;
  }
  animate->setChange = setChange;
  return true;
}

bool CxgAnimate::setEasingFunc(int id, float (*easingFunc)(float x)) {
  struct CxgAnimateStruct* animate = getAnimate(id);
  if(!animate) {
    return false;
  }
  animate->easingFunc = easingFunc;
  return true;
}

bool CxgAnimate::setEndCallback(int id, bool (*endCallback)(int id)) {
  struct CxgAnimateStruct* animate = getAnimate(id);
  if(!animate) {
    return false;
  }
  animate->endCallback = endCallback;
  return true;
}

bool CxgAnimate::setDurationTime(int id, uint16_t durationTime) {
  struct CxgAnimateStruct* animate = getAnimate(id);
  if(!animate) {
    return false;
  }
  animate->durationTime = durationTime;
  return true;
}

bool CxgAnimate::setLoopCount(int id, uint16_t loopCount) {
  struct CxgAnimateStruct* animate = getAnimate(id);
  if(!animate) {
    return false;
  }
  animate->loopCount = loopCount;
  return true;
}

bool CxgAnimate::setDirection(int id, int8_t direction) {
  struct CxgAnimateStruct* animate = getAnimate(id);
  if(!animate) {
    return false;
  }
  animate->direction = direction;
  return true;
}

bool CxgAnimate::setKeyFrameMode(int id, bool isKeyFrameMode) {
  struct CxgAnimateStruct* animate = getAnimate(id);
  if(!animate) {
    return false;
  }
  animate->isKeyFrameMode = isKeyFrameMode;
  return true;
}

//取消执行 如果不指定animateId,则全部清空, 默认全部清空
bool CxgAnimate::clear(int id) {
  if(arrLen == 0) {
    return true;
  }
  for(int i = 0; i < arrLen; i++) {
    if(arr[i] != NULL) {
      if(id == 0 || arr[i]->id == id) {
        free(arr[i]);
        arr[i] = NULL;
        num--;
        if(id != 0) {
          return true;
        }
      }
    }
  }
  if(id == 0) {
    if(arr != NULL) {
      free(arr);
    }
    arr = NULL;
    num = 0;
    arrLen = 0;
    return true;
  }
  return setSize();
}

int CxgAnimate::getNumber() {
  return num;
}

int CxgAnimate::getHeapMemorySize() {
  return sizeof(struct CxgAnimate*) * arrLen + sizeof(struct CxgAnimate) * num;
}