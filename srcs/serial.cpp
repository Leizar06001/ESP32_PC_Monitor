#include "main.hpp"

HANDLE openSerialPort(const char* portName) {
#ifdef UNICODE
    // Convert the port name to a wide character string
    std::wstring widePortName;
    int numChars = MultiByteToWideChar(CP_UTF8, 0, portName, -1, NULL, 0);
    if (numChars > 0) {
        widePortName.resize(numChars);
        MultiByteToWideChar(CP_UTF8, 0, portName, -1, &widePortName[0], numChars);
    } else {
        std::cerr << "Error: Failed to convert port name to wide character string." << std::endl;
        return nullptr;
    }

    // Open the serial port using the wide character string version of CreateFile
    HANDLE hSerial = CreateFileW(widePortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
    // Open the serial port using the ASCII version of CreateFile
    HANDLE hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif

    if (hSerial == INVALID_HANDLE_VALUE) {
        DWORD errorCode = GetLastError();
        std::cerr << "Error: Failed to open serial port (" << portName << "). Error code: " << errorCode << std::endl;
        return nullptr;
    }

    return hSerial;
}

bool configureSerialPort(HANDLE hSerial) {
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error getting serial port state." << std::endl;
        return false;
    }

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error setting serial port state." << std::endl;
        return false;
    }

    // Set timeouts
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "Error setting timeouts." << std::endl;
        return false;
    }

    return true;
}

bool writeDataToSerial(HANDLE hSerial, const char* data, bool debug) {
    DWORD bytes_written;
	if (debug)
		std::cout << ">>> TO SERIAL : " << data << std::endl;
    if (!WriteFile(hSerial, data, strlen(data), &bytes_written, NULL)) {
        std::cerr << "Error writing to serial port." << std::endl;
        return false;
    }
    return true;
}

void closeSerialPort(HANDLE hSerial) {
    CloseHandle(hSerial);
}


