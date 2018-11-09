#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../src/MyLib/RTL_SDR_RadarLib/RTL_SDR_Reciver/RTL_SDR_Reciver.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    IReciverDevice* _reciver;
};

#endif // MAINWINDOW_H
