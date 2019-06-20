#include "Core.h"
#include <QApplication>


Core::Core(QObject *parent) : QObject(parent)
{

    QFile File(QApplication::applicationDirPath()+"/../../../import/style.qss");
    if(File.open(QFile::ReadOnly))
    {
        QString StyleSheet = QLatin1String(File.readAll());
        qApp->setStyleSheet(StyleSheet);
    }
    else
        qDebug()<<"error load QSS file.Need filepath"<<QApplication::applicationDirPath()+"/import/style.qss";

    ServiceLocator::provide(QSharedPointer<ICarrierClass>( new Carrier(false)) );

    _device = QSharedPointer<IReciverDevice>(new RTL_SDR_Reciver());
    _device->openDevice();

    _demodulator = QSharedPointer<IDemodulator>(new Demodulator(QSharedPointer<IPoolObject>()));

    _dataController = new DataController(_device,_demodulator);
    _dataController->run();

    _poolObjects = QSharedPointer<IPoolObject>(new PoolObject());

    if(_mainWindow)
        _mainWindow->subscribe(_poolObjects);

    QObject::connect(&_timer,SIGNAL(timeout()),this,SLOT(slotTimeout()));
    _timer.start(500);

}

Core::~Core()
{
    _timer.stop();

    _dataController->stop();
    delete _dataController;

    _poolObjects.clear();
    delete _mainWindow;
}

void Core::init()
{
    ServiceLocator::provide(QSharedPointer<ICarrierClass>( new NullCarrier()) );

    _device = QSharedPointer<IReciverDevice>(new RTL_SDR_Reciver());
    _device->openDevice();

    _demodulator = QSharedPointer<IDemodulator>(new Demodulator(nullptr));

    _dataController = new DataController(_device,_demodulator);
    _dataController->run();

    _mainWindow  = new MainWindow();
    _mainWindow->show();

    QObject::connect(&_timer,SIGNAL(timeout()),this,SLOT(slotTimeout()));
    _timer.start(1000);
}

void Core::slotTimeout()
{
    if(_device && !_device->isOpenDevice())
        _device->openDevice();
}


