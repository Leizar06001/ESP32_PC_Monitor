#pragma once

#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <signal.h>

HANDLE openSerialPort(const char* portName);
bool configureSerialPort(HANDLE hSerial);
bool writeDataToSerial(HANDLE hSerial, const char* data, bool debug);
void closeSerialPort(HANDLE hSerial);

bool ExtApp_SharedMem_ReadBuffer(char* bu, DWORD bu_size);
std::map<std::string, std::string> parseSensorData(const std::string& data, const std::string& type);

