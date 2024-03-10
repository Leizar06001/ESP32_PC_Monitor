#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "worker.h"
#include <QLabel>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showNormal();
    void connectTray();
    void disconnectTray();

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void handleResults(const std::string &result);
    void handleWorkFinished();
    void handleWorkStarted();
    void handleSerialStatus(bool connected);

    void onStartButtonClicked();
    void onStopButtonClicked();
    void onUpdateList();
    void onAccept();
    void onStartupCheck();

protected:
    void changeEvent(QEvent *event) override;

private:
    Ui::MainWindow  *ui;
    QSystemTrayIcon *trayIcon;
    QMenu           *trayMenu;

    void            setPortsList();
    const QString   timestamp();
    void            initSensorSerials();
    void            createNewWorker();
    void            saveSettings();
    void            loadSettings();
    bool            isLaunchedAtStartup();

    Worker *worker;
    std::map<std::string, QLabel*> sensorLabels;
    std::map<std::string, std::string> sensorsSerialNames;
    std::map<std::string, bool> sensorsActive;
};
#endif // MAINWINDOW_H
