#ifndef MAIN_HPP
#define MAIN_HPP

#include <string>
#include <map>
#include <windows.h>

std::map<std::string, std::string> scanSerialPorts();
bool ExtApp_SharedMem_ReadBuffer(char* bu, DWORD bu_size);
std::map<std::string, std::map<std::string, std::string>> parseSensorData(const std::string& data);


#endif // MAIN_HPP
