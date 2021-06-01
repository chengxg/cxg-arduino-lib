#include "cxg_EventBus.h"

CxgEventBus::CxgEventBus(int intSize) {
  if(intSize > 10) {
    intSize = 10;
  }
  if(intSize < 0) {
    intSize = 0;
  }
  arrLen = intSize;
  arr = ( struct EventBusStruct** )malloc(sizeof(struct EventBusStruct*) * arrLen);
  if(arr != NULL) {
    for(int i = 0; i < arrLen; i++) {
      arr[i] = NULL;
    }
  }
}

CxgEventBus::~CxgEventBus() {
  clearAll();
}

bool CxgEventBus::setSize() {
  //扩容
  if(num > arrLen) {
    int newArrLen = arrLen + arrLen * 0.5 + 1;
    struct EventBusStruct** newArr = ( struct EventBusStruct** )malloc(
      sizeof(struct EventBusStruct*) * newArrLen);
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
    struct EventBusStruct** newArr = ( struct EventBusStruct** )malloc(
      sizeof(struct EventBusStruct*) * newArrLen);

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

bool CxgEventBus::addEvent(char* name, void (*callback)(void* parameter), bool isOnce, bool cover) {
  if(name == NULL) {
    return false;
  }
  struct EventBusStruct* event = ( EventBusStruct* )malloc(sizeof(EventBusStruct));
  event->callback = callback;
  event->name = name;
  event->isOnce = isOnce;
  num++;

  if(cover) {
    for(int i = 0; i < arrLen; i++) {
      //清空已经存在的事件
      if(arr[i] != NULL) {
        if(strcmp(arr[i]->name, name) == 0) {
          free(arr[i]);
          arr[i] = NULL;
          num--;
        }
      }
    }
  }

  bool isSuccess = setSize();
  if(!isSuccess) {
    free(event);
    num--;
    return false;
  }

  for(int i = 0; i < arrLen; i++) {
    //添加到数组中
    if(arr[i] == NULL) {
      arr[i] = event;
      return true;
    }
  }
  free(event);
  num--;
  return false;
}

bool CxgEventBus::on(char* name, void (*callback)(void* parameter), bool cover) {
  return addEvent(name, callback, false, cover);
}

bool CxgEventBus::once(char* name, void (*callback)(void* parameter), bool cover) {
  return addEvent(name, callback, true, cover);
}

void CxgEventBus::emit(char* name, void* parameter) {
  for(int i = 0; i < arrLen; i++) {
    if(arr[i] != NULL && arr[i]->callback != NULL) {
      if(strcmp(arr[i]->name, name) == 0) {
        arr[i]->callback(parameter);
        if(arr[i]->isOnce) {
          free(arr[i]);
          arr[i] = NULL;
          num--;
        }
      }
    }
  }
}

void CxgEventBus::off(char* name, void (*callback)(void* parameter)) {
  for(int i = 0; i < arrLen; i++) {
    if(arr[i] != NULL && arr[i]->callback != NULL) {
      if(strcmp(arr[i]->name, name) == 0 && arr[i]->callback == callback) {
        free(arr[i]);
        arr[i] = NULL;
        num--;
      }
    }
  }
}

void CxgEventBus::clearAll() {
  if(arrLen == 0) {
    return;
  }
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

int CxgEventBus::getNumber() {
  return num;
}

int CxgEventBus::getHeapMemorySize() {
  return sizeof(struct EventBusStruct*) * arrLen + sizeof(struct EventBusStruct) * num;
}