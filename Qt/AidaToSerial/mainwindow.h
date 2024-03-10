#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "worker.h"
#include <QLabel>

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

private slots:
    void handleResults(const std::string &result);
    void handleWorkFinished();
    void handleWorkStarted();
    void onStartButtonClicked();
    void onStopButtonClicked();
    void onUpdateList();
    void onAccept();

private:
    Ui::MainWindow *ui;

    void            setPortsList();
    const QString   timestamp();
    void            initSensorSerials();

    Worker *worker;
    std::map<std::string, QLabel*> sensorLabels;
    std::map<std::string, std::string> sensorsSerialNames;
    std::map<std::string, bool> sensorsActive;
};
#endif // MAINWINDOW_H
