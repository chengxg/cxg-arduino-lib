#include "cxg_JSTime.h"

static unsigned int zero = 0;
static int timeoutMaxId = ~zero / 2 - 2;       //timeoutId为正数
static int timeIntervalMinId = -timeoutMaxId;  //timeInterval为负数

JSTime::JSTime() {
  timeArrLen = 4;
  timeArr = ( struct JSTimeStruct** )malloc(sizeof(struct JSTimeStruct*) * timeArrLen);
  for(int i = 0; i < timeArrLen; i++) {
    timeArr[i] = NULL;
  }
}

JSTime::~JSTime() {
  clearTime(0);
  free(timeArr);
  timeNum = 0;
  timeArrLen = 0;
}

void JSTime::refresh() {
  unsigned long t = millis();
  setSize();

  for(int i = 0; i < timeArrLen; i++) {
    if(timeArr[i] != NULL) {
      //时间溢出问题不大
      if(timeArr[i]->periodTime >= 0 && t - timeArr[i]->startTime >= timeArr[i]->periodTime) {
        if(timeArr[i]->callback != NULL) {
          timeArr[i]->callback();
        }
        //防止timeArr[i]被清空
        if(timeArr[i] == NULL) {
          return;
        }
        if(!timeArr[i]->isInterval) {
          // setTimeout 执行完毕就销毁
          free(timeArr[i]);
          timeArr[i] = NULL;
          timeNum--;
        } else {
          // setInteval 不断进行
          timeArr[i]->startTime = t;
        }
      }
    }
  }
}

void JSTime::setSize() {
  //扩容
  if(timeNum > timeArrLen) {
    int newTimeArrLen = timeArrLen + timeArrLen * 0.5;
    struct JSTimeStruct** newTimeArr = ( struct JSTimeStruct** )malloc(
      sizeof(struct JSTimeStruct*) * newTimeArrLen);

    for(int i = 0; i < newTimeArrLen; i++) {
      if(i < timeArrLen) {
        newTimeArr[i] = timeArr[i];
      } else {
        newTimeArr[i] = NULL;
      }
    }
    free(timeArr);
    timeArr = newTimeArr;
    timeArrLen = newTimeArrLen;
  }

  //缩容
  if((timeArrLen / 2 >= 4) && timeNum <= timeArrLen / 4) {
    int newTimeArrLen = timeArrLen / 2;
    struct JSTimeStruct** newTimeArr = ( struct JSTimeStruct** )malloc(
      sizeof(struct JSTimeStruct*) * newTimeArrLen);

    for(int i = 0; i < newTimeArrLen; i++) {
      newTimeArr[i] = NULL;
    }
    int j = 0;
    for(int i = 0; i < timeArrLen; i++) {
      if(timeArr[i] != NULL) {
        newTimeArr[j] = timeArr[i];
        j++;
      }
    }
    free(timeArr);
    timeArr = newTimeArr;
    timeArrLen = newTimeArrLen;
  }
}

int JSTime::setTimeout(void (*callback)(), unsigned long time) {
  struct JSTimeStruct* t = ( JSTimeStruct* )malloc(sizeof(JSTimeStruct));
  t->callback = callback;
  t->periodTime = time;
  t->startTime = millis();
  t->isInterval = false;
  if(createTimeoutId > timeoutMaxId) {
    createTimeoutId = 1000;
  }
  createTimeoutId++;
  t->id = createTimeoutId;
  timeNum++;
  setSize();

  for(int i = 0; i < timeArrLen; i++) {
    if(timeArr[i] == NULL) {
      timeArr[i] = t;
      return t->id;
    }
  }
  free(t);
  return -1;
}

int JSTime::setInterval(void (*callback)(), unsigned long time) {
  struct JSTimeStruct* t = ( JSTimeStruct* )malloc(sizeof(JSTimeStruct));
  t->callback = callback;
  t->periodTime = time;
  t->startTime = millis();
  t->isInterval = true;
  if(createTimeIntevalId < timeIntervalMinId) {
    createTimeIntevalId = -1000;
  }
  createTimeIntevalId--;
  t->id = createTimeIntevalId;
  timeNum++;
  setSize();

  for(int i = 0; i < timeArrLen; i++) {
    if(timeArr[i] == NULL) {
      timeArr[i] = t;
      return t->id;
    }
  }
  free(t);
  return -1;
}

//取消执行 如果不指定timeId,则全部取消, 默认全部取消
bool JSTime::clearTime(int timeoutId) {
  for(int i = 0; i < timeArrLen; i++) {
    if(timeArr[i] != NULL) {
      if(timeoutId == 0 || timeArr[i]->id == timeoutId) {
        free(timeArr[i]);
        timeArr[i] = NULL;
        timeNum--;
        return true;
      }
    }
  }
  return false;
}