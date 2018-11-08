#ifndef INETWORKWORKER
#define INETWORKWORKER

#include <QObject>

class IPackageController;
class ILogger;

class INetworkWorker: public QObject
{
    Q_OBJECT

public:
    virtual ~INetworkWorker(){}

    virtual void setLogger(ILogger* log) = 0;
    virtual void setController(IPackageController*) = 0;

public slots:
    virtual int64_t writeDatagramm(const QByteArray &byteArray) = 0;
    virtual int64_t readDatagramm() = 0;
signals:
    void finished();
};
#endif // INETWORKWORKER

