/*
 *  The MIT License (MIT)
 * 
 *  Copyright (c) 2021-2022 by Chengxg
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */
#include "cxg_JSTime.h"

static unsigned int zero = 0;
static int timeoutMaxId = ~zero / 2 - 2;       //timeoutId为正数
static int timeIntervalMinId = -timeoutMaxId;  //timeInterval为负数

JSTime::JSTime(int initSize) {
  if(initSize > 20) {
    initSize = 20;
  }
  if(initSize < 1) {
    initSize = 1;
  }
  num = 0;
  arrLen = initSize;
  arr = ( struct JSTimeStruct** )malloc(sizeof(struct JSTimeStruct*) * arrLen);
  if(arr != NULL) {
    for(int i = 0; i < arrLen; i++) {
      arr[i] = NULL;
    }
  }
}

JSTime::~JSTime() {
  for(int i = 0; i < arrLen; i++) {
    if(arr[i] != NULL) {
      free(arr[i]);
      arr[i] = NULL;
    }
  }
  if(arr != NULL) {
    free(arr);
  }
  arr = NULL;
  num = 0;
  arrLen = 0;
}

void JSTime::refresh() {
  unsigned long t = micros();
  for(int i = 0; i < arrLen; i++) {
    if(arr[i] != NULL && arr[i]->id != 0) {
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
        //重新获取时间, 避免回调函数中新增JSTime, 而造成startTime比t大的bug
        t = micros();
        //防止在回调函数里调用了 clearTime 而引发bug
        if(isFree && arr != NULL && i < arrLen && arr[i] != NULL && arr[i]->id == id) {
          // setTimeout 执行完毕就销毁, id置0假释放, 不用free, 结构体进行下次复用
          arr[i]->id = 0;
          num--;
        }
      }
    }
  }
  setSize();
}

bool JSTime::setSize() {
  //扩容
  if(num > arrLen) {
    int newArrLen = arrLen + arrLen * 0.5 + 1;
    if(newArrLen < 4) {
      newArrLen = 4;
    }
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
    //释放原来的引用,替换新的引用
    if(arr != NULL) {
      free(arr);
    }
    arr = newArr;
    arrLen = newArrLen;
  }

  //缩容
  if(arrLen >= 10 && num <= arrLen / 3) {
    int newArrLen = arrLen / 2 + 1;
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
      if(arr[i] != NULL && arr[i]->id != 0) {
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
  struct JSTimeStruct* t = NULL;
  bool isNewStruct = false;
  for(int i = 0; i < arrLen; i++) {
    //找出失效的 结构体进行复用
    if(arr[i] != NULL && arr[i]->id == 0) {
      t = arr[i];
      break;
    }
  }
  if(t == NULL) {
    //没有的话申请一个
    t = ( JSTimeStruct* )malloc(sizeof(JSTimeStruct));
    isNewStruct = true;
  }

  t->simpleCallback = simpleCallback;
  t->callback = callback;
  t->parameterInt = parameterInt;
  t->parameter = parameter;
  t->periodTime = time;
  t->startTime = micros();
  t->isInterval = false;
  if(createTimeoutId > timeoutMaxId) {
    createTimeoutId = 1000;
  }
  createTimeoutId++;
  num++;
  if(!setSize()) {
    if(isNewStruct) {
      free(t);
      t = NULL;
    }
    num--;
    return 0;
  }

  while(true) {
    //检测是否有id重复
    bool isDuplicateId = false;
    for(int i = 0; i < arrLen; i++) {
      if(arr[i] != NULL && arr[i]->id == createTimeoutId) {
        isDuplicateId = true;
        break;
      }
    }
    //有重复的重新分配
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

  if(isNewStruct) {
    for(int i = 0; i < arrLen; i++) {
      if(arr[i] == NULL) {
        t->id = createTimeoutId;
        arr[i] = t;
        return t->id;
      }
    }
  } else {
    t->id = createTimeoutId;
    return t->id;
  }

  if(isNewStruct) {
    free(t);
    t = NULL;
  }
  num--;
  return 0;
}

int JSTime::setTimeout(void (*simpleCallback)(), double time) {
  return baseSetTimeout(simpleCallback, NULL, time * 1000, 0, NULL);
}

int JSTime::setTimeout(void (*callback)(int parameterInt, void* parameter), double time, int parameterInt, void* parameter) {
  return baseSetTimeout(NULL, callback, time * 1000, parameterInt, parameter);
}

int JSTime::baseSetInterval(void (*simpleCallback)(), void (*callback)(int parameterInt, void* parameter), unsigned long time, int parameterInt, void* parameter) {
  struct JSTimeStruct* t = NULL;
  bool isNewStruct = false;
  for(int i = 0; i < arrLen; i++) {
    //找出失效的结构体进行复用
    if(arr[i] != NULL && arr[i]->id == 0) {
      t = arr[i];
      break;
    }
  }
  if(t == NULL) {
    //没有的话申请一个
    t = ( JSTimeStruct* )malloc(sizeof(JSTimeStruct));
    isNewStruct = true;
  }

  t->simpleCallback = simpleCallback;
  t->callback = callback;
  t->parameterInt = parameterInt;
  t->parameter = parameter;
  t->periodTime = time;
  t->startTime = micros();
  t->isInterval = true;
  if(createTimeIntevalId < timeIntervalMinId) {
    createTimeIntevalId = -1000;
  }
  createTimeIntevalId--;
  num++;

  if(!setSize()) {
    if(isNewStruct) {
      free(t);
      t = NULL;
    }
    num--;
    return 0;
  }

  while(true) {
    //检测是否有id重复
    bool isDuplicateId = false;
    for(int i = 0; i < arrLen; i++) {
      if(arr[i] != NULL && arr[i]->id == createTimeIntevalId) {
        isDuplicateId = true;
        break;
      }
    }
    //有重复的重新分配
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

  if(isNewStruct) {
    for(int i = 0; i < arrLen; i++) {
      if(arr[i] == NULL) {
        t->id = createTimeIntevalId;
        arr[i] = t;
        return t->id;
      }
    }
  } else {
    t->id = createTimeIntevalId;
    return t->id;
  }

  if(isNewStruct) {
    free(t);
    t = NULL;
  }
  num--;
  return 0;
}

int JSTime::setInterval(void (*simpleCallback)(), double time) {
  return baseSetInterval(simpleCallback, NULL, time * 1000, 0, NULL);
}

int JSTime::setInterval(void (*callback)(int parameterInt, void* parameter), double time, int parameterInt, void* parameter) {
  return baseSetInterval(NULL, callback, time * 1000, parameterInt, parameter);
}

//取消执行
bool JSTime::clearTime(int timeoutId) {
  if(arrLen == 0) {
    return true;
  }
  if(timeoutId == 0) {
    return true;
  }
  for(int i = 0; i < arrLen; i++) {
    if(arr[i] != NULL) {
      if(arr[i]->id == timeoutId) {
        arr[i]->id = 0;
        num--;
        return true;
      }
    }
  }
  return setSize();
}

//取消执行,可以取消多个定时器
bool JSTime::clearTime(const int* timeoutIds, int start, int length) {
  if(arrLen == 0) {
    return true;
  }
  if(timeoutIds == NULL) {
    return true;
  }
  if(length == 0) {
    return true;
  }
  int end = start + length;
  for(int k = start; k < end; k++) {
    if(timeoutIds[k] == 0) {
      continue;
    }
    for(int i = 0; i < arrLen; i++) {
      if(arr[i] != NULL) {
        if(arr[i]->id == timeoutIds[k]) {
          arr[i]->id = 0;
          num--;
        }
      }
    }
  }
  return setSize();
}

bool JSTime::clearAllTime() {
  if(arrLen == 0) {
    return true;
  }

  for(int i = 0; i < arrLen; i++) {
    if(arr[i] != NULL) {
      if(arr[i]->id != 0) {
        arr[i]->id = 0;
        num--;
      }
    }
  }
  return setSize();
}

int JSTime::getNumber() {
  return num;
}

int JSTime::getHeapMemorySize() {
  int sum = sizeof(JSTime) + sizeof(struct JSTimeStruct*) * arrLen;
  int timeSize = sizeof(struct JSTimeStruct);
  for(int i = 0; i < arrLen; i++) {
    if(arr[i] != NULL) {
      sum += timeSize;
    }
  }
  return sum;
}