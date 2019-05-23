#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "ILogger.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QSharedPointer<ILogger> _logger;
    QTimer _timer;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setLogger(QSharedPointer<ILogger> log) { _logger = log; }
private:
    Ui::MainWindow *ui;

private slots:
    void updateTerminal();
};

#endif // MAINWINDOW_H
