#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QProgressBar>

#include "ILogger.h"
#include "widget/led/led.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QSharedPointer<ILogger> _logger;
    QTimer _timer;
    Led* led;
    QProgressBar* pgDbgMesgFIFO;
    int32_t timeout = 500;
    uint8_t COUNT_MSG_MIN = 10;
    uint8_t COUNT_MSG_MAX = 25;
    uint8_t COUNTER_BALANCE = 10;

    int32_t MAX_INTERVAL = 1000;
    int32_t MIN_INTERVAL = 10;
    int32_t COUNT_TEXT_LINE = 1000;
    void logBalancing();
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setLogger(QSharedPointer<ILogger> log) { _logger = log; }
    void setOpenDevState(bool state);
private:
    Ui::MainWindow *ui;

private slots:
    void updateTerminal();
};

#endif // MAINWINDOW_H
