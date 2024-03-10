#include "mainwindow.h"
#include <QIcon>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icon.png"));
    MainWindow w;
    w.show();
    return a.exec();
}
