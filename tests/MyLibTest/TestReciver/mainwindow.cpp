#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _reciver = new RTL_SDR_Reciver();
    _reciver->openDevice();

    _series = new QLineSeries();
    _series->setUseOpenGL(true);
    _chart = new QtCharts::QChart();
    _chart->addSeries(_series);
    _chart->setTitle("Zoom in/out example");
    _chart->setAnimationOptions(QChart::SeriesAnimations);
    _chart->legend()->hide();
    _chart->createDefaultAxes();

    _chartView = new QChartView(_chart);
    _chartView->setRenderHint(QPainter::Antialiasing);

    setCentralWidget(_chartView);
    resize(800, 600);
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);

    connect(&_timer,SIGNAL(timeout()),this,SLOT(timeout()));
    _timer.start(10);

    QLineSeries *series = dynamic_cast<QLineSeries*>(_chart->series().first());
    _chart->axisX(series)->setMax(8192);
    _chart->axisY(series)->setMax(180);
    _chart->axisY(series)->setMin(-10);
}

MainWindow::~MainWindow()
{
    _timer.stop();
    _chart->removeSeries(_series);
    delete  _series;
    delete _chart;
    delete  _chartView;

    _reciver->closeDevice();
    delete _reciver;
    delete ui;
}

void MainWindow::timeout()
{
    QVector<uint8_t> dataVector = _reciver->getDataBlock(8192*2);

    if(_chart->series().isEmpty())
        return;

    QLineSeries *series = dynamic_cast<QLineSeries*>(_chart->series().first());
    if (series == nullptr)
        return;
    series->clear();

    double yAxisMax = 0.0, yAxisMin = 0.0;
    int xAxis = 8192;
    for (int i = 0, j = 0; i < xAxis; i++, j+=2)
    {

        QPointF p((qreal) i,sqrt((dataVector.at(j) - 127) * (dataVector.at(j) - 127) +
                                  (dataVector.at(j+1) - 127) * (dataVector.at(j+1) -127)));

//        QPointF p((qreal) i,(dataVector.at(j) - 127));

        *series << p;
        yAxisMax = qMax(yAxisMax,p.ry());
        yAxisMin = qMin(yAxisMin,p.ry());
    }
    _chartView->repaint();
}
