#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./main.hpp"
#include <map>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QLineEdit>
#include "QCheckBox"
#include "QThread"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initSensorSerials();
    onUpdateList();

    worker = new Worker();
    connect(worker, &Worker::resultReady, this, &MainWindow::handleResults);
    connect(worker, &Worker::workStarted, this, &MainWindow::handleWorkStarted);
    connect(worker, &Worker::workFinished, this, &MainWindow::handleWorkFinished);
    connect(worker, &Worker::workFinished, this, [this](){ // Delete worker when work is finished
        worker->deleteLater();
        worker = nullptr;
    });

    connect(ui->btnScan, &QPushButton::clicked, this, &MainWindow::setPortsList);
    connect(ui->btnStart, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked);
    connect(ui->btnUpdateList, &QPushButton::clicked, this, &MainWindow::onUpdateList);
    connect(ui->btnAccept, &QPushButton::clicked, this, &MainWindow::onAccept);

    setPortsList();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (worker) {
        worker->deleteLater();
    }
}

void MainWindow::setPortsList() {
    std::map<std::string, std::string> lst = scanSerialPorts();
    ui->boxPorts->clear();
    for (const auto& it : lst) {
        std::cout << it.first << std::endl;
        ui->boxPorts->addItem(QString::fromStdString(it.first));
        ui->miniTerm->append(timestamp() + QString::fromStdString(it.first + " - " + it.second));
    }
}

const QString MainWindow::timestamp(){
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time_t), "%X > ");
    std::string time_str = ss.str();
    return QString::fromStdString(time_str);
}

void MainWindow::onStartButtonClicked()
{
    if (worker) {
        if (worker->isWorking()){
            qDebug() << "Worker is already working.";
            return;
        }
    }
    ui->btnStart->setEnabled(false);

    if (!worker) {
        worker = new Worker();
        connect(worker, &Worker::resultReady, this, &MainWindow::handleResults);
        connect(worker, &Worker::workStarted, this, &MainWindow::handleWorkStarted);
        connect(worker, &Worker::workFinished, this, &MainWindow::handleWorkFinished);
        connect(worker, &Worker::workFinished, this, [this](){ // Delete worker when work is finished
            if (worker) {
                worker->deleteLater();
                worker = nullptr;
            }
        });
    }

    // If worker is not fully deleted, wait for deletion
    if (worker->thread() && worker->thread() != QThread::currentThread()) {
        connect(worker->thread(), &QThread::finished, this, &MainWindow::onStartButtonClicked);
        return;
    }

    string port = ui->boxPorts->currentText().toStdString();
    int delay = ui->boxUpdate->value();
    worker->startWork(sensorsSerialNames, sensorsActive, port, delay);
}

void MainWindow::onStopButtonClicked()
{
    if (worker && worker->isWorking() && !worker->isPendingDelete()) {

        worker->stopWork();
        worker = nullptr;
        qDebug() << "Worker deletion requested.";
    }
    ui->btnStop->setEnabled(false);
}

void MainWindow::handleWorkFinished(){
    qDebug() << "Signal Received Worker Finished";
    ui->btnStart->setEnabled(true);
    ui->boxPorts->setEnabled(true);
}

void MainWindow::handleWorkStarted(){
    qDebug() << "Signal Received Worker Started";
    ui->btnStop->setEnabled(true);
    ui->boxPorts->setEnabled(false);
}

void MainWindow::onUpdateList()
{
    // Clear existing layout
    if (QWidget *scrollWidget = ui->scrollArea->widget()) {
        delete scrollWidget;
    }


    QWidget *scrollContent = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setAlignment(Qt::AlignTop); // Align labels to the top

    const int maxLabelWidth = 28 * ui->scrollList->fontMetrics().averageCharWidth();

    for (const auto& sensorPair : sensorsSerialNames)
    {
        const std::string& sensorName = sensorPair.first;
        const std::string& sensorSerial = sensorPair.second;

        // Crop sensor name if longer than 20 characters
        QString croppedName = QString::fromStdString(sensorName);
        if (croppedName.length() > 25) {
            croppedName = croppedName.left(25) + "...";
        }

        QHBoxLayout *sensorLayout = new QHBoxLayout();
        QLabel *label = new QLabel(croppedName);
        label->setFixedWidth(maxLabelWidth);
        sensorLayout->addWidget(label);

        QLineEdit *textbox = new QLineEdit(QString::fromStdString(sensorSerial));
        textbox->setFixedWidth(100);  // Set a fixed width for text boxes
        sensorLayout->addWidget(textbox);

        QCheckBox *checkbox = new QCheckBox("Active");
        checkbox->setChecked(sensorsActive[sensorName]);
        sensorLayout->addWidget(checkbox);
        // Connect checkbox stateChanged to slot
        connect(checkbox, &QCheckBox::stateChanged, this, [this, sensorName](int state) {
            sensorsActive[sensorName] = (state == Qt::Checked);
        });

        mainLayout->addLayout(sensorLayout);
    }

    mainLayout->addStretch();
    scrollContent->setLayout(mainLayout);

    ui->scrollList->setWidget(scrollContent);
    ui->scrollList->setWidgetResizable(true);  // Allow scroll area to resize its contents

    scrollContent->adjustSize();
}

void MainWindow::onAccept(){

}

void MainWindow::handleResults(const std::string &result)
{
    bool refresh = ui->boxShowVals->checkState();

    QVBoxLayout *layout;
    if (refresh){
        // Get the current scroll position
        int scrollBarValue = ui->scrollArea->verticalScrollBar()->value();

        // Get the existing layout from the scroll area
        QWidget *scrollAreaContent = ui->scrollArea->widget();
        if (scrollAreaContent) {
            layout = dynamic_cast<QVBoxLayout*>(scrollAreaContent->layout());
            if (!layout) {
                // Create a new layout if the existing one is not QVBoxLayout
                layout = new QVBoxLayout(scrollAreaContent);
                layout->setAlignment(Qt::AlignTop);
                scrollAreaContent->setLayout(layout);
            }
        } else {
            // Create a new QWidget if no existing content
            scrollAreaContent = new QWidget(ui->scrollArea);
            layout = new QVBoxLayout(scrollAreaContent);
            layout->setAlignment(Qt::AlignTop);
            scrollAreaContent->setLayout(layout);
            ui->scrollArea->setWidget(scrollAreaContent);
        }

        // Update the scroll position
        ui->scrollArea->verticalScrollBar()->setValue(scrollBarValue);
    }

    auto datas = parseSensorData(result);

    // std::cout << "Create labels" << std::endl;
    for(const auto& types : datas){
        if (refresh){
            if (sensorLabels.find("Type:" + types.first) == sensorLabels.end()){
                QLabel *typeLabel = new QLabel(QString::fromStdString("**** " + types.first + " ****"));
                layout->addWidget(typeLabel);
                sensorLabels["Type:" + types.first] = typeLabel;
            }
        }
        for(const auto& sensor : types.second){
            std::string labelText = types.first + ":" + sensor.first;

            auto it = sensorLabels.find(labelText);
            if (it == sensorLabels.end()){
                if (refresh){
                    // Create a QHBoxLayout to hold the sensor label and value
                    QHBoxLayout *sensorLayout = new QHBoxLayout();

                    QLabel *sensorLabel = new QLabel(QString::fromStdString(sensor.first + ":"));
                    sensorLabel->setObjectName("sensorLabel"); // Set object name for sensor label
                    QLabel *sensorValueLabel = new QLabel(QString::fromStdString(sensor.second));

                    sensorLayout->addWidget(sensorLabel);
                    sensorLayout->addWidget(sensorValueLabel);

                    layout->addLayout(sensorLayout);

                    // Store the sensor label and its QLabel for updating
                    sensorLabels[labelText] = sensorValueLabel;
                }
                if (sensorsSerialNames.find(labelText) == sensorsSerialNames.end())
                    sensorsSerialNames[labelText] = "";
            } else {
                if (refresh)
                    it->second->setText(QString::fromStdString(sensor.second));
            }
        }
    }

    if (refresh){
        // Add a spacer at the end to push labels to the top
        layout->addStretch();

        // Ensure the scroll area updates its layout
        ui->scrollArea->widget()->adjustSize();
    }

}

void MainWindow::initSensorSerials(){
    sensorsSerialNames["sys:Time"] = "time";
    sensorsSerialNames["sys:CPU Utilization"] = "CPUuse";
    sensorsSerialNames["sys:CPU Clock"] = "CPUclock";
    sensorsSerialNames["sys:GPU Utilization"] = "GPUuse";
    sensorsSerialNames["sys:GPU Clock"] = "GPUclock";
    sensorsSerialNames["sys:GPU Used Dedicated Memory"] = "GPUmem";
    sensorsSerialNames["sys:Used Memory"] = "RAMuse";
    sensorsSerialNames["sys:Memory Clock"] = "RAMclock";
    sensorsSerialNames["temp:CPU"] = "CPUtemp";
    sensorsSerialNames["temp:GPU"] = "GPUtemp";
    sensorsSerialNames["temp:Motherboard"] = "MBtemp";
    sensorsSerialNames["temp:GPU Hotspot"] = "GPUhot";
    sensorsSerialNames["pwr:CPU Package"] = "CPUpower";
    sensorsSerialNames["pwr:GPU"] = "GPUpower";
    sensorsSerialNames["fan:Chassis #1"] = "FAN1";
    sensorsSerialNames["fan:Chassis #3"] = "FAN3";
    sensorsSerialNames["fan:Chassis #4"] = "FAN4";
    sensorsSerialNames["fan:Chassis #5"] = "FAN5";
    sensorsSerialNames["fan:Chassis #6"] = "FAN6";
    sensorsSerialNames["fan:CPU"] = "CPUfan";
    sensorsSerialNames["fan:Water Pump"] = "CPUpump";
    sensorsSerialNames["fan:GPU"] = "GPUfan";
    for(auto &sensor : sensorsSerialNames){
        sensorsActive[sensor.first] = true;
    }
}

