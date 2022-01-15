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
  if(length < 5) {
    return;
  }
  if(!(*(buff + length - 3) == '|' || *(buff + length - 6) == '|')) {
    return;
  }

  if(resolveCallback == NULL) {
    return;
  }

  char name = 0;     //最后一个是名称, 就一个字符
  char verify1 = 0;  //倒数第二个是验证码, 两个字符 可有可无
  char verify2 = 0;  //倒数第二个是验证码, 两个字符 可有可无
  int dataEndIndex = length;
  if(*(buff + length - 6) == '|') {
    //有验证码 和 指令项
    name = *(buff + length - 5);
    verify1 = *(buff + length - 3);
    verify2 = *(buff + length - 2);
    dataEndIndex = dataEndIndex - 4;
  } else if(*(buff + length - 3) == '|') {
    //只有指令项, 无验证码
    verify1 = 0;
    verify2 = 0;
    name = *(buff + length - 2);
    dataEndIndex = dataEndIndex - 1;
  }
  if(name == 0) {
    return;
  }
  if(verify1 > 0) {
    uint32_t sum = 0;
    for(int i = 1; i < dataEndIndex; i++) {
      sum += *(buff + i);
    }
    char bodyVerify1 = (sum & 0x00ff) % ('z' - ' ') + ' ';
    char bodyVerify2 = ((sum & 0x00ff00) >> 8) % ('z' - ' ') + ' ';

    if(!(bodyVerify1 == verify1 && bodyVerify2 == verify2)) {
      return;
    }
  }
  if(forwardCallback != NULL) {
    //用于数据转发
    bool isContinue = forwardCallback(name, ( char* )buff, length);
    if(!isContinue) {
      return;
    }
  }

  char body[dataEndIndex - 2] = {0};  //字符串数据部分
  memset(body, 0, dataEndIndex - 2);
  dataEndIndex = dataEndIndex - 2;  //去掉 |name
  for(int i = 1; i < dataEndIndex; i++) {
    body[i - 1] = *(buff + i);
  }

  resolveCallback(name, body, dataEndIndex - 1);
}

void CxgCommandString::addData(byte data) {
  //重新开始
  if(data == '{') {
    *buff = data;
    count = 1;
    return;
  }
  if(count < 1) {
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

  //结束字符
  if(data == '}') {
    resolveCommand(count);
    count = 0;
  }
}

//发送指令
void CxgCommandString::sendCommand(char name, const char* body, bool isVerify) {
  int length = strlen(body);
  char command[length + 8] = {0};
  memset(command, 0, length + 8);
  command[0] = '{';

  for(int i = 0; i < length; i++) {
    command[i + 1] = *(body + i);
  }

  int strLength = 0;
  if(isVerify) {
    command[length + 1] = '|';
    command[length + 2] = name;
    //计算校验和
    uint32_t sum = 0;
    int verifyLen = length + 3;
    for(int i = 1; i < verifyLen; i++) {
      sum += *(command + i);
    }

    command[length + 3] = '|';
    command[length + 4] = (sum & 0x00ff) % ('z' - ' ') + ' ';
    command[length + 5] = ((sum & 0x00ff00) >> 8) % ('z' - ' ') + ' ';
    command[length + 6] = '}';
    strLength = length + 7;
  } else {
    command[length + 1] = '|';
    command[length + 2] = name;
    command[length + 3] = '}';
    strLength = length + 4;
  }

  if(sendCallback != NULL) {
    sendCallback(command, strLength);
  }
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