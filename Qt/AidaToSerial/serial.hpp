#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <Windows.h>
#include <string>
#include <map>

using namespace std;


void EnumerateValues(HKEY hKey, DWORD numValues, map<wstring, wstring>& resultMap);
void EnumerateSubKeys(HKEY RootKey, const WCHAR* subKey, map<wstring, wstring>& resultMap, unsigned int tabs = 0);
void convertMapToStdString(map<wstring, wstring>& resultMap, map<string, string>& resultStringMap);
map<string, string> scanSerialPorts();
HANDLE openSerialPort(const char* portName);
bool configureSerialPort(HANDLE hSerial);
bool writeDataToSerial(HANDLE hSerial, const char* data, bool debug);
void closeSerialPort(HANDLE hSerial) ;

#endif // SERIAL_HPP
