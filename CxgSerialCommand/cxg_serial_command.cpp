#include "cxg_serial_command.h"

/**
 * 解析命令, 不能超过256个字符
 * @params {char *} str 需要解析的字符串指针 格式 {name, body, time, verify}
 * @params {function} resolveCommand 解析完成后的回调函数, 将返回 name, body, time, verify 
 */
void parseCommand(char* str, void (*resolveCommand)(char*, char*, char*, int, HardwareSerial), HardwareSerial serial) {
  int len = strlen(str);
  boolean isStart = false;
  boolean isEnd = false;
  byte splitIndex = 0;
  int charIndex = 0;

  char name[32] = {0};   //指令名
  char body[200] = {0};  //数据
  char time[6] = {0};    //时间
  char verify[6] = {0};  //验证码

  for(int i = 0; i < len; i++) {
    char c = str[i];
    if(c == '{') {
      isStart = true;
      continue;
    }
    if(!isStart) {
      continue;
    }
    if(c == '}') {
      isEnd = true;
      break;
    }
    if(c == ',') {
      splitIndex++;
      charIndex = 0;
      continue;
    }
    if(splitIndex == 0) {
      if(charIndex >= 31) {
        break;
      }
      name[charIndex] = c;
      charIndex++;
    }
    if(splitIndex == 1) {
      if(charIndex >= 199) {
        break;
      }
      body[charIndex] = c;
      charIndex++;
    }
    if(splitIndex == 2) {
      if(charIndex >= 5) {
        break;
      }
      time[charIndex] = c;
      charIndex++;
    }
    if(splitIndex == 3) {
      if(charIndex >= 5) {
        break;
      }
      verify[charIndex] = c;
      charIndex++;
    }
  }

  int verifyInt = 0;
  if(strlen(verify)) {
    verifyInt = atol(verify);
  }

  if(isEnd && strlen(name) > 0) {
    resolveCommand(name, body, time, verifyInt, serial);
  }
}

/**
 * 从串口中 读取控制指令
 */
void readCommandData(HardwareSerial serial, void (*resolveCommand)(char*, char*, char*, int, HardwareSerial)) {
  if(serial.available() > 0) {
    int count = 0;
    char str[256];
    boolean isStart = false;
    int available = serial.available();
    while(available > 0) {
      available--;
      char d = serial.read();
      if(d == 0) {
        continue;
      }
      if(d == '{') {
        isStart = true;
        count = 0;
      }
      if(!isStart) {
        continue;
      }
      str[count] = d;
      count++;
      if(d == '}') {
        isStart = false;
        parseCommand(str, resolveCommand, serial);
        break;
      }
      if(count == 256) {
        break;
      }
      if(available == 0) {
        delay(1);  //延时, 待接收完数据
        available = serial.available();
        //等待一段时间后仍然没有接受到数据, 则退出
        if(available == 0) {
          break;
        }
      }
    }
  }
}

/**
 * 序列化命令
 * @params {String} name 命令名
 * @params {String} data 信息主体
 * @params {String} time 5位时间数字, 默认为当前时间
 * @params {boolean} verify 是否需要验证码, 信息主体的ascii码的和, 用作验证
 * @return {String} 返回生成的命令
 */
char* serializeCommand(char* command, char* name, char* data, char* time, boolean verify) {
  strcat(command, "{");
  strcat(command, name);
  strcat(command, ",");
  strcat(command, data);
  if(verify) {
    strcat(command, ",");
    if(time != NULL && strlen(time) > 0) {
      strcat(command, time);
    } else {
      int t = millis() % 65535;
      char tStr[6] = {0};
      itoa(t, tStr, 10);
      strcat(command, tStr);
    }
    int ver = calcStrSum(data);
    char verStr[6] = {0};
    itoa(ver, verStr, 10);
    strcat(command, ",");
    strcat(command, verStr);
  }
  strcat(command, "}");
  return command;
}

/**
 * 从数据体中获取参数值
 * 
 */
char* getCommandParam(char* des, char* urlQuery, char* paramsName) {
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
int getCommandParamToInt(char* urlQuery, char* paramsName) {
  char dataChar[11] = {0};
  getCommandParam(dataChar, urlQuery, paramsName);
  if(strlen(dataChar)) {
    return atol(dataChar);
  }
  return INT32_MAX;
}

void strcatInt(char* des, int num) {
  char numStr[11] = {0};
  itoa(num, numStr, 10);
  strcat(des, numStr);
}

/**
 * 计算校验和
 */
int calcStrSum(char* str) {
  int sum = 0;
  int len = strlen(str);
  for(int i = 0; i < len; i++) {
    sum += str[i];
  }
  return sum;
}