#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    _reciver = new RTL_SDR_Reciver();
    _reciver->openDevice();

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    _reciver->closeDevice();
    delete _reciver;
    delete ui;
}
