#include "cxg_EventBus.h"

CxgEventBus::CxgEventBus() {
  eventBusArrLen = 4;
  eventBusArr = ( struct EventBusStruct** )malloc(sizeof(struct EventBusStruct*) * eventBusArrLen);
  for(int i = 0; i < eventBusArrLen; i++) {
    eventBusArr[i] = NULL;
  }
}

CxgEventBus::~CxgEventBus() {
  for(int i = 0; i < eventBusArrLen; i++) {
    if(eventBusArr[i] != NULL) {
      free(eventBusArr[i]);
    }
  }
  free(eventBusArr);
  eventBusNum = 0;
  eventBusArrLen = 0;
}

void CxgEventBus::setSize() {
  //扩容
  if(eventBusNum > eventBusArrLen) {
    int newEventBusLen = eventBusArrLen + eventBusArrLen * 0.5;
    struct EventBusStruct** newEventBusArr = ( struct EventBusStruct** )malloc(
      sizeof(struct EventBusStruct*) * newEventBusLen);

    for(int i = 0; i < newEventBusLen; i++) {
      if(i < eventBusArrLen) {
        newEventBusArr[i] = eventBusArr[i];
      } else {
        newEventBusArr[i] = NULL;
      }
    }
    free(eventBusArr);
    eventBusArr = newEventBusArr;
    eventBusArrLen = newEventBusLen;
  }

  //缩容
  if((eventBusArrLen / 2 >= 4) && eventBusNum <= eventBusArrLen / 4) {
    int newEventBusLen = eventBusArrLen / 2;
    struct EventBusStruct** newEventBusArr = ( struct EventBusStruct** )malloc(
      sizeof(struct EventBusStruct*) * newEventBusLen);

    for(int i = 0; i < newEventBusLen; i++) {
      newEventBusArr[i] = NULL;
    }
    int j = 0;
    for(int i = 0; i < eventBusArrLen; i++) {
      if(eventBusArr[i] != NULL) {
        newEventBusArr[j] = eventBusArr[i];
        j++;
      }
    }
    free(eventBusArr);
    eventBusArr = newEventBusArr;
    eventBusArrLen = newEventBusLen;
  }
}

bool CxgEventBus::addEvent(char* name, void (*callback)(void* parameter), bool isOnce, bool cover) {
  if(name == NULL) {
    return false;
  }
  struct EventBusStruct* event = ( EventBusStruct* )malloc(sizeof(EventBusStruct));
  event->callback = callback;
  event->name = name;
  event->isOnce = isOnce;
  eventBusNum++;

  if(cover) {
    for(int i = 0; i < eventBusArrLen; i++) {
      //清空已经存在的事件
      if(eventBusArr[i] != NULL) {
        if(strcmp(eventBusArr[i]->name, name) == 0) {
          free(eventBusArr[i]);
          eventBusArr[i] = NULL;
          eventBusNum--;
        }
      }
    }
  }

  setSize();
  for(int i = 0; i < eventBusArrLen; i++) {
    //添加到数组中
    if(eventBusArr[i] == NULL) {
      eventBusArr[i] = event;
      return true;
    }
  }
  free(event);
  eventBusNum--;
  return false;
}

bool CxgEventBus::on(char* name, void (*callback)(void* parameter), bool cover) {
  return addEvent(name, callback, false, cover);
}

bool CxgEventBus::once(char* name, void (*callback)(void* parameter), bool cover) {
  return addEvent(name, callback, true, cover);
}

void CxgEventBus::emit(char* name, void* parameter) {
  for(int i = 0; i < eventBusArrLen; i++) {
    if(eventBusArr[i] != NULL && eventBusArr[i]->callback != NULL) {
      if(strcmp(eventBusArr[i]->name, name) == 0) {
        eventBusArr[i]->callback(parameter);
        if(eventBusArr[i]->isOnce) {
          free(eventBusArr[i]);
          eventBusArr[i] = NULL;
          eventBusNum--;
        }
      }
    }
  }
}

void CxgEventBus::off(char* name, void (*callback)(void* parameter)) {
  for(int i = 0; i < eventBusArrLen; i++) {
    if(eventBusArr[i] != NULL && eventBusArr[i]->callback != NULL) {
      if(strcmp(eventBusArr[i]->name, name) == 0 && eventBusArr[i]->callback == callback) {
        free(eventBusArr[i]);
        eventBusArr[i] = NULL;
        eventBusNum--;
      }
    }
  }
}

void CxgEventBus::clear() {
  for(int i = 0; i < eventBusArrLen; i++) {
    if(eventBusArr[i] != NULL) {
      free(eventBusArr[i]);
      eventBusArr[i] = NULL;
    }
  }
  eventBusNum = 0;
  setSize();
}
