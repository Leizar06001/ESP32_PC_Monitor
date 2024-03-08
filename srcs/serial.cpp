#include "main.hpp"

HANDLE openSerialPort(const char* portName) {
    HANDLE hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening serial port." << std::endl;
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
