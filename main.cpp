#include <QApplication>
#include <QPushButton>
#include <QDebug>
#include "mainwindow.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow mainwindow;

    mainwindow.show();
    return QApplication::exec();
}
