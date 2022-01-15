#include "cxg_Command.h"

CxgCommand::CxgCommand() {}

CxgCommand::~CxgCommand() {
  if(buff != NULL) {
    free(buff);
    buff = NULL;
  }
  if(startBuff != NULL) {
    free(startBuff);
    startBuff = NULL;
  }
  if(endBuff != NULL) {
    free(endBuff);
    endBuff = NULL;
  }
}

void CxgCommand::setStart(byte* st, unsigned int size) {
  if(startBuff != NULL) {
    free(startBuff);
    startBuff = NULL;
  }
  startBuff = ( byte* )malloc(sizeof(byte) * size);
  startSize = size;
  memcpy(startBuff, st, size);
}

void CxgCommand::setEnd(byte* ed, unsigned int size) {
  if(endBuff != NULL) {
    free(endBuff);
    endBuff = NULL;
  }
  endBuff = ( byte* )malloc(sizeof(byte) * size);
  endSize = size;
  memcpy(endBuff, ed, size);
}

bool CxgCommand::setBufferSize(unsigned int size) {
  buff = ( byte* )malloc(sizeof(byte) * size);
  if(buff == NULL) {
    return false;
  }
  buffSize = size;
  memset(buff, 0, size);
  return true;
}

bool CxgCommand::checkCommand(int checkIndex) {
  if(!isStart) {
    return false;
  }
  if(checkIndex < 3) {
    return false;
  }
  int len = *(buff + checkIndex);
  int compareIndex = checkIndex - len - startSize;
  if(compareIndex < 0) {
    return false;
  }
  return isMatchStart(compareIndex);
}

bool CxgCommand::isMatchStart(int compareIndex) {
  if(compareIndex < 0) {
    return false;
  }
  bool isSameStart = true;
  for(int i = 0; i < startSize; i++) {
    if(*(buff + compareIndex + i) != *(startBuff + i)) {
      isSameStart = false;
      break;
    }
  }
  return isSameStart;
}

bool CxgCommand::isMatchEnd(int compareIndex) {
  bool isSameEnd = true;
  for(int i = 0; i < endSize; i++) {
    if(*(buff + compareIndex + i) != *(endBuff + i)) {
      isSameEnd = false;
      break;
    }
  }
  return isSameEnd;
}

void CxgCommand::setResolveCommandCallback(void (*resolveCommand)(byte* buff, int startIndex, int length)) {
  this->resolveCommandCallback = resolveCommand;
}

void CxgCommand::setResolveCommandParamCallback(void (*resolveCommand)(byte* buff, int startIndex, int length, void* param), void* param) {
  this->resolveCommandParamCallback = resolveCommand;
  this->resolveCommandCallbackParameter = param;
}

void CxgCommand::setSendCommandCallback(void (*sendCommand)(byte* buff, int length)) {
  this->sendCommandCallback = sendCommand;
}

void CxgCommand::addData(byte data) {
  if(!isStart) {
    if(count == 0 && data != *startBuff) {
      return;
    }
    *(buff + count) = data;
    count++;
    if(count >= startSize) {
      //检测开始
      bool isMatch = isMatchStart(count - startSize);
      if(isMatch) {
        isStart = true;
      }
    }
    return;
  }

  if(count >= buffSize) {
    count = 0;
    isStart = false;
  }

  *(buff + count) = data;
  count++;

  if(isStart && count >= endSize + startSize + 2) {
    if(data == *(endBuff + endSize - 1)) {
      //检测结束
      int compareIndex = count - endSize;
      bool isMatch = isMatchEnd(compareIndex);
      if(isMatch) {
        int checkIndex = compareIndex - 1;
        if(checkCommand(checkIndex)) {
          isStart = false;
          count = 0;
          if(resolveCommandParamCallback != NULL) {
            int len = *(buff + checkIndex);
            resolveCommandParamCallback(buff, checkIndex - len, len, resolveCommandCallbackParameter);
            return;
          }
          //结束
          if(resolveCommandCallback != NULL) {
            int len = *(buff + checkIndex);
            resolveCommandCallback(buff, checkIndex - len, len);
          }
        }
      }
    }
  }
}

void CxgCommand::sendCommand(byte* command, int length) {
  if(sendCommandCallback == NULL) {
    return;
  }
  if(length > 255 || length < 0) {
    return;
  }
  sendCommandCallback(startBuff, startSize);
  sendCommandCallback(command, length);
  byte lengthByte = length;
  sendCommandCallback(&lengthByte, 1);
  sendCommandCallback(endBuff, endSize);
}

uint32_t CxgCommand::getVerifySum(byte* data, int start, int length) {
  //计算校验和
  uint32_t sum = 0;
  int end = start + length;
  for(int i = start; i < end; i++) {
    sum += *(data + i);
  }
  return sum;
}
