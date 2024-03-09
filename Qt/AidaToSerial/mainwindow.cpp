#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./main.hpp"
#include <map>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnScan, &QPushButton::clicked, this, &MainWindow::setPortsList);

    setPortsList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPortsList() {
    std::map<std::string, std::string> lst = scanSerialPorts();
    ui->boxPorts->clear();
    for (const auto& it : lst) {
        std::cout << it.first << std::endl;
        ui->boxPorts->addItem(QString::fromStdString(it.first));
    }
}
