#include "cxg_Command_String.h"

CxgCommandString::CxgCommandString() {}

CxgCommandString::~CxgCommandString() {
  if(buff != NULL) {
    free(buff);
    buff = NULL;
  }
}

bool CxgCommandString::setBufferSize(unsigned int size) {
  buff = ( byte* )malloc(sizeof(byte) * size);
  if(buff == NULL) {
    return false;
  }
  buffSize = size;
  memset(buff, 0, size);
  return true;
}

void CxgCommandString::setResolveCallback(void (*resolveCallback)(char name, char* body, int length)) {
  this->resolveCallback = resolveCallback;
}

void CxgCommandString::setSendCallback(void (*sendCallback)(char* buff, int length)) {
  this->sendCallback = sendCallback;
}

void CxgCommandString::setForwardCallback(bool (*forwardCallback)(char name, char* buff, int length)) {
  this->forwardCallback = forwardCallback;
}

void CxgCommandString::resolveCommand(int length) {
  if(length < 7) {
    return;
  }
  if(*(buff + length - 4) != '|') {
    return;
  }

  if(resolveCallback == NULL) {
    return;
  }

  char name = *(buff + length - 3);  //最后一个是名称, 就一个字符
  char verify = 0;                   //最后一个是验证码, 就一个字符
  int endIndex = length - 4;
  if(*(buff + length - 6) == '|') {
    verify = *(buff + length - 5);
    endIndex = endIndex - 2;
  }
  if(verify > 0) {
    int sum = 0;
    for(int i = 2; i < endIndex; i++) {
      sum += *(buff + i);
    }
    char bodyVerify = sum % ('z' - ' ') + ' ';
    if(bodyVerify != verify) {
      return;
    }
  }
  if(forwardCallback != NULL) {
    bool isContinue = forwardCallback(name, ( char* )buff, length);
    if(!isContinue) {
      return;
    }
  }

  char body[endIndex] = {0};  //字符串数据部分
  memset(body, 0, endIndex);

  for(int i = 2; i < endIndex; i++) {
    body[i - 2] = *(buff + i);
  }

  resolveCallback(name, body, endIndex - 2);
}

void CxgCommandString::addData(byte data) {
  //重新开始
  if(data == '{') {
    if(count == 1 || count == 2) {
      *(buff + 1) = data;
      count = 2;
      return;
    }

    *buff = data;
    count = 1;
    return;
  }
  if(count < 2) {
    count = 0;
    return;
  }

  if(count >= buffSize) {
    count = 0;
    return;
  }
  //过滤掉其他非字符
  if(data < ' ' || data > '}') {
    count = 0;
    return;
  }

  *(buff + count) = data;
  count++;

  if(data == '}') {
    if(*(buff + count - 2) == '}') {
      resolveCommand(count);
      count = 0;
    }
  }
}

//发送指令
void CxgCommandString::sendCommand(char name, const char* body, bool isVerify) {
  int length = strlen(body);
  char command[length + 10] = {0};
  memset(command, 0, length + 10);
  command[0] = '{';
  command[1] = '{';

  for(int i = 0; i < length; i++) {
    command[i + 2] = *(body + i);
  }

  int strLength = 0;
  if(isVerify) {
    command[length + 2] = '|';
    command[length + 3] = calcVerify(body, 0, length);
    command[length + 4] = '|';
    command[length + 5] = name;
    command[length + 6] = '}';
    command[length + 7] = '}';
    strLength = length + 8;
  } else {
    command[length + 2] = '|';
    command[length + 3] = name;
    command[length + 4] = '}';
    command[length + 5] = '}';
    strLength = length + 6;
  }

  if(sendCallback != NULL) {
    sendCallback(command, strLength);
  }
}

char CxgCommandString::calcVerify(const char* body, int start, int length) {
  int sum = 0;
  for(int i = start; i < start + length; i++) {
    sum += *(body + i);
  }
  return sum % ('z' - ' ') + ' ';
}

/**
 * 从数据体中获取参数值
 */
char* CxgCommandString::getQueryParam(char* des, const char* urlQuery, const char* paramsName) {
  int urlQueryLen = strlen(urlQuery);
  int paramsNameLen = strlen(paramsName);

  for(int i = 0; i < urlQueryLen; i++) {
    if(i >= urlQueryLen - paramsNameLen) {
      break;
    }
    int start = i;
    boolean isSame = true;
    for(int j = 0; j < paramsNameLen; j++) {
      if(urlQuery[start] != paramsName[j]) {
        isSame = false;
        break;
      }
      start++;
    }
    if(isSame) {
      i = start;
      if(urlQuery[i] == '=') {
        i++;
        while(urlQuery[i]) {
          if(urlQuery[i] == '&') {
            return des;
          }
          *des++ = urlQuery[i];
          i++;
          if(i > 1000) {
            break;
          }
        }
        return des;
      }
    }
  }
  return des;
}

/**
 * 从数据体中获取参数值, 并转化成int类型返回
 * 
 */
int CxgCommandString::getQueryParamToInt(const char* urlQuery, const char* paramsName) {
  char dataChar[11] = {0};
  CxgCommandString::getQueryParam(dataChar, urlQuery, paramsName);
  if(strlen(dataChar)) {
    return atol(dataChar);
  }
  return INT32_MAX;
}

void CxgCommandString::strcatInt(char* des, int num) {
  char numStr[11] = {0};
  itoa(num, numStr, 10);
  strcat(des, numStr);
}

void CxgCommandString::resSuccess(char name, int id, const char* data) {
  char body[128] = {0};
  memset(body, 0, 128);
  strcat(body, "id=");
  strcatInt(body, id);
  strcat(body, "&");
  strcat(body, data);
  sendCommand(name, body);
}

void CxgCommandString::resError(char name, int id, const char* errMsg) {
  char body[128] = {0};
  memset(body, 0, 128);
  strcat(body, "id=");
  strcatInt(body, id);
  strcat(body, "&msg=");
  strcat(body, errMsg);
  sendCommand(name, body);
}