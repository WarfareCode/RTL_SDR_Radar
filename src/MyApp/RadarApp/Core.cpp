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

    _poolObjects = QSharedPointer<IPoolObject>(new PoolObject());

    _mainWindow.subscribe(_poolObjects);

    QObject::connect(&_timer,SIGNAL(timeout()),this,SLOT(slotTimeout()));
    _timer.start(1000);
}

Core::~Core()
{
    _poolObjects.clear();
}

void Core::init()
{
    _mainWindow.show();
}

void Core::slotTimeout()
{
}


