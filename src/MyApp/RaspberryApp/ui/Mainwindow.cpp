#include "Mainwindow.h"
#include "ui_Mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(&_timer, &QTimer::timeout,
                     this, &MainWindow::updateTerminal);

    ui->teTerminal->setTextColor(QColor(0,180,0));
    _timer.start(500);
}

MainWindow::~MainWindow()
{
    _logger.clear();
    delete ui;
}

void MainWindow::updateTerminal()
{
    if(!_logger.isNull() && !_logger->isEmpty())
        ui->teTerminal->append(_logger->pop());

}
