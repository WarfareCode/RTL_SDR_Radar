#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QtCore/QtMath>
#include <QtCharts/QChartView>

#include "../src/MyLib/RTL_SDR_RadarLib/RTL_SDR_Reciver/RTL_SDR_Reciver.h"
#include "../MyLib/RTL_SDR_RadarLib/PoolObject/PoolObject.h"
#include "../MyLib/RTL_SDR_RadarLib/Logger/Logger.h"
#include "../MyLib/RTL_SDR_RadarLib/Carrier/Carrier.h"
#include "../MyLib/RTL_SDR_RadarLib/Carrier/ServiceLocator.h"
#include "../MyLib/RTL_SDR_RadarLib/DataController/DataController.h"

using complex = std::complex<double>;

namespace Ui {
class MainWindow;
}

QT_CHARTS_USE_NAMESPACE

#include <fftw3.h>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void remove_dc(complex *data, int length);
private:
    Ui::MainWindow *ui;
    complex *inbuf, *outbuf;
    fftw_plan plan;

    const int N = 2048;
    QTimer _timer;

    QChart* _chart;
    QChartView* _chartView;
    QLineSeries* _series;


    IDataController* _dataController = nullptr;
    QSharedPointer<IReciverDevice> _device = nullptr;
public slots:
    void timeout();
};

#endif // MAINWINDOW_H
