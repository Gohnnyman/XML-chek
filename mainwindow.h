//
// Created by Gohnny on 04.06.2021.
//

#ifndef XML_CHEK_MAINWINDOW_H
#define XML_CHEK_MAINWINDOW_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:

    [[maybe_unused]] void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};


#endif //XML_CHEK_MAINWINDOW_H
