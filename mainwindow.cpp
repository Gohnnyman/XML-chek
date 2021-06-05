//
// Created by Gohnny on 04.06.2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QFileDialog>

#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

[[maybe_unused]]
void MainWindow::on_pushButton_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this,"Select XML/txt file",
    QDir::currentPath(), "XML files (*.xml);;Text files (*.txt)");
    QFile file(file_path);



}

MainWindow::~MainWindow() {
    delete ui;
}

