#include "cxg_JSTime.h"

static unsigned int zero = 0;
static int timeoutMaxId = ~zero / 2 - 2;       //timeoutId为正数
static int timeIntervalMinId = -timeoutMaxId;  //timeInterval为负数

JSTime::JSTime(int intSize) {
  if(intSize > 10) {
    intSize = 10;
  }
  if(intSize < 0) {
    intSize = 0;
  }
  arrLen = intSize;
  arr = ( struct JSTimeStruct** )malloc(sizeof(struct JSTimeStruct*) * arrLen);
  if(arr != NULL) {
    for(int i = 0; i < arrLen; i++) {
      arr[i] = NULL;
    }
  }
}

JSTime::~JSTime() {
  clearTime(0);
}

void JSTime::refresh() {
  unsigned long t = millis();
  for(int i = 0; i < arrLen; i++) {
    if(arr[i] != NULL) {
      //时间溢出问题不大
      if(arr[i]->periodTime >= 0 && t - arr[i]->startTime >= arr[i]->periodTime) {
        bool isFree = false;
        int id = arr[i]->id;
        if(!arr[i]->isInterval) {
          // setTimeout 执行完毕就销毁
          isFree = true;
        } else {
          // setInteval 不断进行
          arr[i]->startTime = t;
        }

        if(arr[i]->callback != NULL) {
          arr[i]->callback(arr[i]->parameterInt, arr[i]->parameter);
        } else if(arr[i]->simpleCallback != NULL) {
          arr[i]->simpleCallback();
        }
        //防止在回调函数里调用了 clearTime 而引发bug
        if(isFree && arr != NULL && arr[i] != NULL && arr[i]->id == id) {
          // setTimeout 执行完毕就销毁
          free(arr[i]);
          arr[i] = NULL;
          num--;
        }
      }
    }
  }
}

bool JSTime::setSize() {
  //扩容
  if(num > arrLen) {
    int newArrLen = arrLen + arrLen * 0.5 + 1;
    struct JSTimeStruct** newArr = ( struct JSTimeStruct** )malloc(
      sizeof(struct JSTimeStruct*) * newArrLen);
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
    struct JSTimeStruct** newArr = ( struct JSTimeStruct** )malloc(
      sizeof(struct JSTimeStruct*) * newArrLen);
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

int JSTime::baseSetTimeout(void (*simpleCallback)(), void (*callback)(int parameterInt, void* parameter), unsigned long time, int parameterInt, void* parameter) {
  struct JSTimeStruct* t = ( JSTimeStruct* )malloc(sizeof(JSTimeStruct));
  t->simpleCallback = simpleCallback;
  t->callback = callback;
  t->parameterInt = parameterInt;
  t->parameter = parameter;
  t->periodTime = time;
  t->startTime = millis();
  t->isInterval = false;
  if(createTimeoutId > timeoutMaxId) {
    createTimeoutId = 1000;
  }
  createTimeoutId++;
  num++;
  t->id = createTimeoutId;
  bool isSuccess = setSize();
  if(!isSuccess) {
    free(t);
    return 0;
  }

  //防止id重复
  bool isDuplicateId = false;
  while(true) {
    for(int i = 0; i < arrLen; i++) {
      if(arr[i] != NULL && arr[i]->id == createTimeoutId) {
        isDuplicateId = true;
        break;
      }
    }
    //概率极小
    if(isDuplicateId) {
      if(createTimeoutId > timeoutMaxId) {
        createTimeoutId = 1000;
      }
      createTimeoutId++;
      t->id = createTimeoutId;
      isDuplicateId = false;
    } else {
      break;
    }
  }

  for(int i = 0; i < arrLen; i++) {
    if(arr[i] == NULL) {
      arr[i] = t;
      return t->id;
    }
  }
  free(t);
  return 0;
}

int JSTime::setTimeout(void (*simpleCallback)(), unsigned long time) {
  return baseSetTimeout(simpleCallback, NULL, time, 0, NULL);
}

int JSTime::setTimeout(void (*callback)(int parameterInt, void* parameter), unsigned long time, int parameterInt, void* parameter) {
  return baseSetTimeout(NULL, callback, time, parameterInt, parameter);
}

int JSTime::baseSetInterval(void (*simpleCallback)(), void (*callback)(int parameterInt, void* parameter), unsigned long time, int parameterInt, void* parameter) {
  struct JSTimeStruct* t = ( JSTimeStruct* )malloc(sizeof(JSTimeStruct));
  t->simpleCallback = simpleCallback;
  t->callback = callback;
  t->parameterInt = parameterInt;
  t->parameter = parameter;
  t->periodTime = time;
  t->startTime = millis();
  t->isInterval = true;
  if(createTimeIntevalId < timeIntervalMinId) {
    createTimeIntevalId = -1000;
  }
  createTimeIntevalId--;
  num++;
  t->id = createTimeIntevalId;
  bool isSuccess = setSize();
  if(!isSuccess) {
    free(t);
    return 0;
  }

  //防止id重复
  bool isDuplicateId = false;
  while(true) {
    for(int i = 0; i < arrLen; i++) {
      if(arr[i] != NULL && arr[i]->id == createTimeIntevalId) {
        isDuplicateId = true;
        break;
      }
    }
    //概率极小
    if(isDuplicateId) {
      if(createTimeIntevalId < timeIntervalMinId) {
        createTimeIntevalId = -1000;
      }
      createTimeIntevalId--;
      t->id = createTimeIntevalId;
      isDuplicateId = false;
    } else {
      break;
    }
  }

  for(int i = 0; i < arrLen; i++) {
    if(arr[i] == NULL) {
      arr[i] = t;
      return t->id;
    }
  }
  free(t);
  return 0;
}

int JSTime::setInterval(void (*simpleCallback)(), unsigned long time) {
  return baseSetInterval(simpleCallback, NULL, time, 0, NULL);
}

int JSTime::setInterval(void (*callback)(int parameterInt, void* parameter), unsigned long time, int parameterInt, void* parameter) {
  return baseSetInterval(NULL, callback, time, parameterInt, parameter);
}

//取消执行 如果不指定timeId,则全部取消, 默认全部取消
bool JSTime::clearTime(int timeoutId) {
  if(arrLen == 0) {
    return true;
  }
  for(int i = 0; i < arrLen; i++) {
    if(arr[i] != NULL) {
      if(timeoutId == 0 || arr[i]->id == timeoutId) {
        free(arr[i]);
        arr[i] = NULL;
        num--;
        if(timeoutId != 0) {
          return true;
        }
      }
    }
  }
  if(timeoutId == 0) {
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

int JSTime::getNumber() {
  return num;
}

int JSTime::getHeapMemorySize() {
  return sizeof(struct JSTimeStruct*) * arrLen + sizeof(struct JSTimeStruct) * num;
}