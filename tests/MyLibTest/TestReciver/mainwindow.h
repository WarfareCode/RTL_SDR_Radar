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


namespace Ui {
class MainWindow;
}

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    IReciverDevice* _reciver;
    QTimer _timer;

    QChart* _chart;
    QChartView* _chartView;
    QLineSeries* _series;

public slots:
    void timeout();
};

#endif // MAINWINDOW_H
