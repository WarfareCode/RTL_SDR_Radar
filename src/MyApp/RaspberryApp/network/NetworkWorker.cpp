#include <QString>
#include <QHostAddress>

#include "NetworkWorker.h"

NetworkWorker::NetworkWorker(const QString &ip, uint16_t port)
{
    _isConnected = false;
    connect(ip,port);
    qDebug() << "create NetworkWorker";
}

NetworkWorker::~NetworkWorker()
{
    disconnect();
    qDebug() << "delete NetworkWorker";
}

void NetworkWorker::addDebugMsg(const QString &str)
{
    if(_log)
        _log->push(str);
}

bool NetworkWorker::connect(const QString &ip, uint16_t port, uint16_t timeout)
{
    if(isConnected())
        disconnect();

    QString logString = QString("[TCP] try to connect to: IP = %1, port = %2 ..../").arg(ip).arg(port);

    addDebugMsg(logString);

    _socket = std::unique_ptr<QTcpSocket>(new QTcpSocket());

    if(!_socket)
    {
        logString = QString("Error create QTcpSocket");
        addDebugMsg(logString);
        return false;
    }

    if( _socket->state() != QAbstractSocket::ConnectedState &&
       _socket->state() != QAbstractSocket::ConnectingState)
    {
        _socket->connectToHost(ip, port);
        if(!_socket->waitForConnected(timeout))
        {
            logString = QString( "[TCP] get error string from connection: %1 ").arg(_socket->errorString());
            addDebugMsg(logString);
            return false;
        }
        _isConnected = true;
    }
    return  _isConnected;
}

void NetworkWorker::disconnect()
{
    if(_socket)
    {
        _socket->disconnectFromHost();
        _socket.reset(nullptr);
        _isConnected = false;
        addDebugMsg("[TCP] disconnect");
    }
}

int64_t NetworkWorker::writeDatagramm(const QByteArray &byteArray)
{
    if(_socket == nullptr)
        return  -1;

    //qDebug() << "[TCP] write data:" <<  "[" <<  byteArray.toHex() << "]";
    int64_t ret = _socket->write(byteArray);

    addDebugMsg(QString("[TCP] size write data : %1").arg(ret));

    _socket->flush();
    return ret;
}

int64_t NetworkWorker::writeDatagramm(QString& ip,
                                      uint16_t port,
                                      const QByteArray &byteArray)
{
    if(_socket == nullptr)
        return  -1;

    if( (_socket->peerAddress().toString() != ip) &&
        (_socket->peerPort() != port ))
    {
        disconnect();
        connect(ip,port);
    }

    if(!isConnected())
        return -1;

    return writeDatagramm(byteArray);
}

