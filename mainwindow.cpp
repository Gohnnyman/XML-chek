//
// Created by Gohnny on 04.06.2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "XMLChecker.h"
#include <QMessageBox>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

[[maybe_unused]]
void MainWindow::on_pushButton_clicked()
{

    QDir dir;
    dir.cdUp();
    dir.cd("test");
    QString path = dir.absolutePath();

    QString file_path = QFileDialog::getOpenFileName(this,"Select XML/txt file",
        path, "XML files (*.xml);;Text files (*.txt)");
    QFile file(file_path);
    XMLChecker::check(file);

}

MainWindow::~MainWindow() {
    delete ui;
}

