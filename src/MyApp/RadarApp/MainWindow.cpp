#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //создание виджета для вывода обстановки и всей графики
    double size = 945;
//    _graphicsWidget = new GraphicsWidget(size,
//                                         QSharedPointer<IPoolObject>(),
//                                         this);
    ///ui->vrlRadar->addWidget(_graphicsWidget);
    ui->teTerminal->setTextColor(QColor(0,180,0));
    ui->teTerminal->insertPlainText("test....");
}

MainWindow::~MainWindow()
{
    if(_graphicsWidget)
    {
        delete  _graphicsWidget;
        _graphicsWidget = nullptr;
    }
    delete ui;
}

void MainWindow::subscribe(QSharedPointer<IPoolObject> poolObject)
{
    if(!poolObject.isNull())
        _graphicsWidget->subscribe(poolObject);
}

void MainWindow::showRadarWidget(bool state)
{
        //ui->vrlRadar->widget()->setVisible(state);
}
