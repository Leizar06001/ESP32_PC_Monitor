#include "main.hpp"

using namespace std;

bool stopFlag = false;

void signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << "Ctrl+C pressed. Exiting..." << std::endl;
        stopFlag = true;
    }
}

int main (int argc, char *argv[]){
    signal(SIGINT, signalHandler);

    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <COM port>" << endl;
        return 1;
    }
    const char* portName = argv[1];
    const char* data = "Hello, Serial!\n";

    HANDLE hSerial = openSerialPort(portName);
    if (hSerial == INVALID_HANDLE_VALUE) {
        return 1;
    }
    if (!configureSerialPort(hSerial)) {
        closeSerialPort(hSerial);
        return 1;
    }

    char buffer[4096];
    while (!stopFlag) {
        if (ExtApp_SharedMem_ReadBuffer(buffer, sizeof(buffer))) {

            std::string input(buffer);
            // cout << buffer << endl << endl;

            std::map<std::string, std::string> tempData = parseSensorData(input, "temp");
            std::map<std::string, std::string> fanData = parseSensorData(input, "fan");
            std::map<std::string, std::string> pwrData = parseSensorData(input, "pwr");
            std::map<std::string, std::string> dutyData = parseSensorData(input, "duty");
            std::map<std::string, std::string> voltData = parseSensorData(input, "volt");
            std::map<std::string, std::string> currData = parseSensorData(input, "curr");
            std::map<std::string, std::string> sysData = parseSensorData(input, "sys");


            // std::cout << "Sys Data:" << std::endl;
            // for (const auto& data : sysData) {
            //     std::cout << data.first << ": " << data.second << std::endl;
            // }

            // std::cout << "\nTemperature Data:" << std::endl;
            // for (const auto& data : tempData) {
            //     std::cout << data.first << ": " << data.second << std::endl;
            // }

            // // Print fan data
            // std::cout << "\nFan Data:" << std::endl;
            // for (const auto& data : fanData) {
            //     std::cout << data.first << ": " << data.second << std::endl;
            // }

            // // Print power data
            // std::cout << "\nPower Data:" << std::endl;
            // for (const auto& data : pwrData) {
            //     std::cout << data.first << ": " << data.second << std::endl;
            // }

            // // Print duty data
            // std::cout << "\nDuty Data:" << std::endl;
            // for (const auto& data : dutyData) {
            //     std::cout << data.first << ": " << data.second << std::endl;
            // }

            // // Print voltage data
            // std::cout << "\nVoltage Data:" << std::endl;
            // for (const auto& data : voltData) {
            //     std::cout << data.first << ": " << data.second << std::endl;
            // }

            // // Print current data
            // std::cout << "\nCurrent Data:" << std::endl;
            // for (const auto& data : currData) {
            //     std::cout << data.first << ": " << data.second << std::endl;
            // }

            string strToSerial = "time:" + sysData["Time"] + ";";

            strToSerial += "CPUuse:" + sysData["CPU Utilization"] + ";";
            strToSerial += "CPUclock:" + sysData["CPU Clock"] + ";";
            strToSerial += "CPUtemp:" + tempData["CPU"] + ";";
            strToSerial += "CPUfan:" + fanData["CPU"] + ";";
            strToSerial += "CPUpump:" + fanData["Water Pump"] + ";";
            strToSerial += "CPUpower:" + pwrData["CPU Package"] + ";";

            strToSerial += "GPUuse:" + sysData["GPU Utilization"] + ";";
            strToSerial += "GPUclock:" + sysData["GPU Clock"] + ";";
            strToSerial += "GPUtemp:" + tempData["GPU"] + ";";
            strToSerial += "GPUhot:" + tempData["GPU Hotspot"] + ";";
            strToSerial += "GPUfan:" + fanData["GPU"] + ";";
            strToSerial += "GPUpower:" + pwrData["GPU"] + ";";
            strToSerial += "GPUmem:" + sysData["GPU Used Dedicated Memory"] + ";";

            strToSerial += "RAMuse:" + sysData["Used Memory"] + ";";
            strToSerial += "RAMclock:" + sysData["Memory Clock"] + ";";

            strToSerial += "MBtemp:" + tempData["Motherboard"] + ";";
            strToSerial += "FAN1:" + fanData["Chassis #1"] + ";";
            strToSerial += "FAN3:" + fanData["Chassis #3"] + ";";
            strToSerial += "FAN4:" + fanData["Chassis #4"] + ";";
            strToSerial += "FAN5:" + fanData["Chassis #5"] + ";";
            strToSerial += "FAN6:" + fanData["Chassis #6"] + ";";


            strToSerial += "\n";
            if (!writeDataToSerial(hSerial, strToSerial.c_str(), 0)) {
                closeSerialPort(hSerial);
                return 1;
            }

        }
        // std::cout << "***********************************************" << std::endl;


        Sleep(500);
    }

    closeSerialPort(hSerial);

    return 0;
}
