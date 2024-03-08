#include "main.hpp"

bool ExtApp_SharedMem_ReadBuffer(char* bu, DWORD bu_size) {
    HANDLE th;
    char* mappedData;
    bool result = false;

    th = OpenFileMappingA(FILE_MAP_READ, FALSE, "AIDA64_SensorValues");
    if (th != INVALID_HANDLE_VALUE) {
        mappedData = (char*) MapViewOfFile(th, FILE_MAP_READ, 0, 0, 0);
        if (mappedData != NULL) {
            strncpy(bu, mappedData, bu_size);
            if (UnmapViewOfFile(mappedData))
                result = true;
        }
        CloseHandle(th);
    }

    return result;
}

std::map<std::string, std::string> parseSensorData(const std::string& data, const std::string& type) {
    std::map<std::string, std::string> result;
    size_t startPos = 0;
    while (startPos != std::string::npos) {
        size_t tempStart = data.find("<" + type + "><id>", startPos);
        if (tempStart == std::string::npos) {
            break;  // Exit loop if no more <type><label> tags found
        }

        size_t labelStart = data.find("<label>", tempStart) + 7;  // Length of "<type><label>"
        size_t labelEnd = data.find("</label>", labelStart);
        std::string label = data.substr(labelStart, labelEnd - labelStart);

        size_t valueStart = data.find("<value>", labelEnd);
        size_t valueEnd = data.find("</value>", valueStart);
        std::string value = data.substr(valueStart + 7, valueEnd - valueStart - 7);

        result[label] = value;
        startPos = valueEnd;
    }
    return result;
}
