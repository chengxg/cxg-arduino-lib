#ifndef CXG_SERIAL_COMMAND_H_
#define CXG_SERIAL_COMMAND_H_

#include "Arduino.h"

/**
 *  自定义通用 指令格式
 *  {name, param1=value1&param2=value2, 12345, 2321}
 *  {指令名, 参数1=值1&参数2=值2, 5位时间, 指令体ascii码和}
 */
void readCommandData(HardwareSerial serial, void (*resolveCommand)(char*, char*, char*, int, HardwareSerial));
void parseCommand(char str[], void (*resolveCommand)(char*, char*, char*, int, HardwareSerial), HardwareSerial serial);
char* serializeCommand(char* command, char* name, char* data, char* time, boolean verify);
char* getCommandParam(char* des, char* urlQuery, char* paramsName);
int getCommandParamToInt(char* urlQuery, char* paramsName);
void strcatInt(char* des, int num);
int calcStrSum(char* str);
#endif