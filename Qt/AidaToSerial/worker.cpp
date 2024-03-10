#include "Worker.h"
#include <QDebug>
#include <QThread>
#include "main.hpp"
#include <QTimer>
#include "./serial.hpp"

using namespace std;

Worker::Worker(QObject *parent) : QObject(parent){}

void Worker::updateDatas() {
    char buffer[4096];
    while (!m_pendingDelete){
        if (!connectSerial()){
            qDebug() << "Error connecting to serial";
            emit sendSerialStatus(false);
            QThread::sleep(5);
            continue ;
        }
        emit sendSerialStatus(true);
        while (!m_pendingDelete){
            std::string prev(buffer);

            // Get data from AIDA
            if (ExtApp_SharedMem_ReadBuffer(buffer, sizeof(buffer))) {
                std::string input(buffer);
                if (input != prev){
                    prev = input;
                    // Send it to the main window
                    emit resultReady(input);

                    auto datas = parseSensorData(input);
                    string strToSerial;
                    for(const auto & dt : sensorsActive){
                        if (dt.second){
                            const string serName = sensorsSerialNames[dt.first];
                            size_t sep = dt.first.find(":");
                            const string sensType = dt.first.substr(0, sep);
                            const string sensName = dt.first.substr(sep + 1);;
                            const string val = datas[sensType][sensName];
                            strToSerial += serName + ":" + val + ";";
                        }
                    }
                    strToSerial += "\n";
                    if (!writeDataToSerial(hSerial, strToSerial.c_str(), 0)){
                        closeSerialPort(hSerial);
                        serialConnected = 0;
                        qDebug() << "Error sending to serial";
                        emit sendSerialStatus(false);
                        QThread::sleep(5);
                        break;
                    }
                }
            }
            QThread::msleep(refreshDelay);
        }

    }
    if (serialConnected)
        closeSerialPort(hSerial);


    emit workFinished();
    // qDebug() << "Signal emitted";
    m_isWorking = false;
    QThread::currentThread()->quit();
}

bool Worker::connectSerial(){
    qDebug() << "Connect to " << serialPort;
    qDebug() << "Refresh " << refreshDelay;
    hSerial = openSerialPort(serialPort.c_str());
    if (hSerial == INVALID_HANDLE_VALUE)
        return 0;
    if (!configureSerialPort(hSerial)){
        closeSerialPort(hSerial);
        return 0;
    }
    serialConnected = 1;
    return 1;
}

void Worker::getNewDatas(map<string, string> sensSerialNames,
                         map<string, bool> sensActive){
    sensorsSerialNames = sensSerialNames;
    sensorsActive = sensActive;
}

void Worker::startWork(map<string, string> sensSerialNames,
                       map<string, bool> sensActive,
                       string port,
                       int delay)
{
    sensorsSerialNames = sensSerialNames;
    sensorsActive = sensActive;
    serialPort = port;
    refreshDelay = delay;

    qDebug() << "Worker: new";
    if (m_isWorking) {
        qDebug() << "Worker is already working.";
        return;
    }
    m_pendingDelete = false;
    m_isWorking = true;

    // If the worker is already running in a thread, wait for it to finish
    if (thread() && thread() != QThread::currentThread()) {
        qDebug() << "Waiting for previous thread to finish...";
        // connect(thread(), &QThread::finished, this, &Worker::startWork);
        return;
    }
    qDebug() << "Worker: new thread";
    // Create a new thread
    QThread* thread = new QThread();

    qDebug() << "Worker: moving";
    // Move the worker object to the new thread
    this->moveToThread(thread);

    // Connect signals and slots
    connect(thread, &QThread::started, this, &Worker::updateDatas);
    connect(this, &Worker::resultReady, thread, &QThread::quit);
    connect(this, &Worker::resultReady, this, [this](){
        // Clean up after work is done
        this->deleteLater();
    });

    connect(thread, &QThread::finished, thread, &QThread::deleteLater); // Delete thread when finished
    connect(thread, &QThread::finished, this, &Worker::workFinished);

    // Start the thread
    thread->start();
    emit workStarted();
}

void Worker::stopWork() {
    m_pendingDelete = true;
    m_isWorking = false;
}

bool Worker::isWorking() const {
    return m_isWorking;
}

bool Worker::isPendingDelete() const
{
    return m_pendingDelete;
}
