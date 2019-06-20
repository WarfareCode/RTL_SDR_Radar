#include "Mainwindow.h"
#include "ui_Mainwindow.h"

#include <QLayout>
#include <QLabel>
#include <QDebug>
#include <QTextBlock>

MainWindow::MainWindow(QWidget *parent) :
                                          QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(&_timer, &QTimer::timeout,
                     this, &MainWindow::updateTerminal);

    ui->teTerminal->setTextColor(QColor(0,180,0));
    _timer.start(timeout);

    QLabel* lRtlSdr = new QLabel("RTL-SDR Reсiver",this);
    QLabel* lLogBuff = new QLabel(tr("Очередь сообщений"),this);

    led = new Led(this);
    led->setMinimumSize(QSize(15,15));
    led->setMaximumSize(QSize(15,15));

    pgDbgMesgFIFO = new QProgressBar(this);
    pgDbgMesgFIFO->setRange(0,100);
    pgDbgMesgFIFO->setValue(0);

    ui->statusBar->addWidget(led);
    ui->statusBar->addWidget(lRtlSdr);
    ui->statusBar->addWidget(lLogBuff);
    ui->statusBar->addWidget(pgDbgMesgFIFO);
}

MainWindow::~MainWindow()
{
    _logger.clear();
    delete ui;
}

void MainWindow::setOpenDevState(bool state)
{
    if(led)
        led->setColor(state ? Qt::green : Qt::red);
}

void MainWindow::updateTerminal()
{
    if(!_logger.isNull() && !_logger->isEmpty())
    {
        ui->teTerminal->append(_logger->pop());
        pgDbgMesgFIFO->setValue(_logger->countMsg());
    }

    logBalancing();

    if(ui->teTerminal->document()->lineCount() > COUNT_TEXT_LINE)
        ui->teTerminal->clear();
}

void MainWindow::logBalancing()
{
    if(_logger.isNull())
        return;

    static uint8_t upBalanceCounter = 0;
    static uint8_t downBalanceCounter = 0;
    int interval = _timer.interval();

    if(_logger->countMsg() < COUNT_MSG_MIN)
    {
        if(downBalanceCounter > COUNTER_BALANCE)
        {
            interval =_timer.interval() * 2;
            if((interval) > MAX_INTERVAL)
                interval = MAX_INTERVAL;
            downBalanceCounter = 0;
        }
        else
            ++downBalanceCounter;
    }

    if((_logger->countMsg() > COUNT_MSG_MAX))
    {
        if(upBalanceCounter > COUNTER_BALANCE)
        {
            interval =_timer.interval() / 2;
            if((interval) < MIN_INTERVAL)
                interval = MIN_INTERVAL;
            upBalanceCounter = 0;
        }
        else
            ++upBalanceCounter;
    }

    if(interval != _timer.interval())
        _timer.setInterval(interval);
}
