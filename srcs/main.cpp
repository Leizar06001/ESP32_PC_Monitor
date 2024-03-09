#include "main.hpp"

using namespace std;

bool stopFlag = false;

void signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << "Ctrl+C pressed. Exiting..." << std::endl;
        stopFlag = true;
    }
}

void error() {
    cout << "Retry in 5 sec..." << endl;
    Sleep(5000);
}

void prtDatas(std::map<std::string, std::string> data, string name){
    std::cout << "*** " << name << " Data ***" << std::endl;
    for (const auto& d : data) {
        std::cout << d.first << ": " << d.second << std::endl;
    }
    cout << endl;
}

int main (int argc, char *argv[]){
    signal(SIGINT, signalHandler);

    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <COM port>" << endl;
        return 1;
    }
    int debug = 0;
    if (argc > 2) {
        debug = atoi(argv[2]);
    }
    const char* portName = argv[1];

    while (!stopFlag){
        cout << "Opening serial port " << portName << endl;
        HANDLE hSerial = openSerialPort(portName);
        if (hSerial == INVALID_HANDLE_VALUE) {
            error();
            continue;
        }
        if (!configureSerialPort(hSerial)) {
            closeSerialPort(hSerial);
            error();
            continue;
        }

        char buffer[4096];
        string prev(buffer);
        cout << ">> Connected, sending data !" << endl;
        while (!stopFlag) {
            if (ExtApp_SharedMem_ReadBuffer(buffer, sizeof(buffer))) {

                std::string input(buffer);
                if (0)
                    cout << buffer << endl << endl;
                if (input != prev){
                    prev = input;
                    std::map<std::string, std::string> tempData = parseSensorData(input, "temp");
                    std::map<std::string, std::string> fanData = parseSensorData(input, "fan");
                    std::map<std::string, std::string> pwrData = parseSensorData(input, "pwr");
                    std::map<std::string, std::string> dutyData = parseSensorData(input, "duty");
                    std::map<std::string, std::string> voltData = parseSensorData(input, "volt");
                    std::map<std::string, std::string> currData = parseSensorData(input, "curr");
                    std::map<std::string, std::string> sysData = parseSensorData(input, "sys");

                    // Print system data
                    if (debug){
                        prtDatas(sysData, "Sys");
                        prtDatas(tempData, "Temp");
                        prtDatas(fanData, "Fan");
                        prtDatas(pwrData, "Pwr");
                        prtDatas(dutyData, "Duty");
                        prtDatas(voltData, "Volt");
                        prtDatas(currData, "Curr");
                    }

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

                    if (!writeDataToSerial(hSerial, strToSerial.c_str(), debug)) {
                        closeSerialPort(hSerial);
                        error();
                        break;
                    }
                    if (debug)
                        std::cout << "***********************************************" << std::endl;
                }
            }

            Sleep(200);
        }
        closeSerialPort(hSerial);
    }

    return 0;
}
