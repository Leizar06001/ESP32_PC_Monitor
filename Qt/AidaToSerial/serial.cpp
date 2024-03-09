#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <iostream>
#include <map>
#include <cstring> // For strlen
#include <cwchar>  // For wcscpy
#include <locale>  // For std::wstring_convert

using namespace std;

BYTE databuffer[256];

void EnumerateValues(HKEY hKey, DWORD numValues, map<wstring, wstring>& resultMap)
{
    DWORD dwIndex = 0;
    LPWSTR valueName = new WCHAR[256];
    DWORD valNameLen;
    DWORD dataType;
    DWORD dataSize = 1024;
    long result;

    for (int i = 0; i < numValues; i++)
    {
        valNameLen = 256;
        dataType = 0;

        // Allocate memory for data
        BYTE* data = new BYTE[dataSize];

        result = RegEnumValue(hKey, dwIndex, valueName, &valNameLen, NULL, &dataType, data, &dataSize);

        if (result != ERROR_SUCCESS) {
            wcout << L"\nError RegEnumValue > " << result;
            delete[] valueName;
            delete[] data;
            return;
        }

        // Store data as a string and name as value in the map
        resultMap[wstring(reinterpret_cast<wchar_t*>(data), dataSize / sizeof(wchar_t))] = wstring(valueName, valNameLen);

        delete[] data;
        dwIndex++;
    }

    delete[] valueName;
}


void EnumerateSubKeys(HKEY RootKey, const WCHAR* subKey, map<wstring, wstring>& resultMap, unsigned int tabs = 0)
{
    HKEY hKey;
    DWORD cSubKeys;
    DWORD maxSubkeyLen;
    DWORD cValues;
    DWORD maxValueLen;
    DWORD retCode;
    long result;

    result = RegOpenKeyEx(RootKey, subKey, 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        wcout << L"\nError RegOpenKeyEx > " << result;
        return;
    }

    result = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, &maxSubkeyLen, NULL, &cValues, &maxValueLen, NULL, NULL, NULL);
    if (result != ERROR_SUCCESS) {
        wcout << L"\nError RegQueryInfoKey > " << result;
        RegCloseKey(hKey);
        return;
    }

    if (cSubKeys > 0)
    {
        WCHAR currentSubkey[MAX_PATH];

        for (int i = 0; i < cSubKeys; i++) {
            DWORD currentSubLen = MAX_PATH;

            retCode = RegEnumKeyEx(hKey, i, currentSubkey, &currentSubLen, NULL, NULL, NULL, NULL);
            if (retCode == ERROR_SUCCESS)
            {
                for (int i = 0; i < tabs; i++)
                    wprintf(L"\t");
                wprintf(L"(%d) %s\n", i + 1, currentSubkey);

                WCHAR subKeyPath[MAX_PATH];
                wcscpy(subKeyPath, subKey);
                wcscat(subKeyPath, L"\\");
                wcscat(subKeyPath, currentSubkey);

                EnumerateSubKeys(RootKey, subKeyPath, resultMap, (tabs + 1));
            }
        }
    }
    else
    {
        EnumerateValues(hKey, cValues, resultMap);
    }

    RegCloseKey(hKey);
}

void convertMapToStdString(map<wstring, wstring>& resultMap, map<string, string>& resultStringMap)
{
    for (const auto& pair : resultMap) {
        int requiredSize = WideCharToMultiByte(CP_UTF8, 0, pair.first.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (requiredSize == 0) {
            cerr << "Error converting to UTF-8." << endl;
            continue;
        }
        string dataString(requiredSize, 0);
        WideCharToMultiByte(CP_UTF8, 0, pair.first.c_str(), -1, &dataString[0], requiredSize, nullptr, nullptr);

        requiredSize = WideCharToMultiByte(CP_UTF8, 0, pair.second.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (requiredSize == 0) {
            cerr << "Error converting to UTF-8." << endl;
            continue;
        }
        string nameString(requiredSize, 0);
        WideCharToMultiByte(CP_UTF8, 0, pair.second.c_str(), -1, &nameString[0], requiredSize, nullptr, nullptr);

        resultStringMap[dataString] = nameString;
    }
}

map<string, string> scanSerialPorts() {
    wcout << L"Scanning ports" << endl;
    map<wstring, wstring> resultMap;
    EnumerateSubKeys(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", resultMap);

    // Convert wstring map to string map
    map<string, string> resultStringMap;
    convertMapToStdString(resultMap, resultStringMap);

    // Print out the results
    // for (const auto& pair : resultStringMap) {
    //     cout << "Value Data: " << pair.first << endl;
    //     cout << "Value Name: " << pair.second << endl;
    //     cout << "-----------------------" << endl;
    // }
    return resultStringMap;
}
