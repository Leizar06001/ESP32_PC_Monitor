#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <windows.h>


using namespace std;

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    bool isWorking() const;
    bool isPendingDelete() const; // New function

signals:
    void resultReady(const string &result);
    void workFinished();
    void workStarted();

public slots:
    void startWork(map<string, string> sensSerialNames,
                   map<string, bool> sensActive,
                   string port,
                   int delay);
    void stopWork();
    void getNewDatas(map<string, string> sensSerialNames,
                     map<string, bool> sensActive);

private:
    void updateDatas();
    bool connectSerial();

    bool m_isWorking = false;
    bool m_pendingDelete = false;
    map<string, string> sensorsSerialNames;
    map<string, bool> sensorsActive;
    string serialPort;
    int refreshDelay;
    HANDLE hSerial;
    bool serialConnected;
};

#endif // WORKER_H
