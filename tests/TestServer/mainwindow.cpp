#include <QDateTime>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../../src/MyLib/RTL_SDR_RadarLib/Demodulator/StructAircraft.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&tcpServer, SIGNAL(newConnection()),
            this, SLOT(acceptConnection()));
     if (tcpServer.listen(QHostAddress::Any, 62000))
         qDebug()<<"listen true";
}

MainWindow::~MainWindow()
{
    tcpServer.close();
    delete ui;
}

void MainWindow::acceptConnection()
{
    qDebug()<<"new connection avalible";
    tcpServerConnection = tcpServer.nextPendingConnection();
    QObject::connect(tcpServerConnection, SIGNAL(readyRead()),
            this, SLOT(updateServerProgress()));
    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
}

void MainWindow::updateServerProgress()
{
    bytesReceived += (int)tcpServerConnection->bytesAvailable();
    QByteArray array = tcpServerConnection->readAll();

    qDebug()<<"bytesReceived = " << bytesReceived << "data" << array.toHex();

    StructAircraft a;

    memcpy((char*)&a,(char*)array.data(),sizeof (StructAircraft));
    QString str;
    str.append( QString("+++++++++++++++++++++++++++++++++++++\n"));
    str.append( QString("ICAO: %1\n").arg(a.icao,9,16));
    str.append( QString("Flight number: %1\n").arg(QString(a.flight)));
    str.append( QString("Altitude: %1\n").arg(a.altitude/VALUE_LSB));
    str.append( QString("Speed: %1\n").arg(a.speed / VALUE_LSB));
    str.append( QString("Course: %1\n").arg(a.course / VALUE_LSB));
    str.append( QString("Longitude: %1\n").arg(a.lon * LON_VALUE_LSB ));
    str.append( QString("Latitude: %1\n").arg(a.lat * LAT_VALUE_LSB));
    str.append( QString("Time last update: %1\n")
                   .arg(QDateTime::fromMSecsSinceEpoch(a.seen).toString("hh:mm:ss.zzz")));
    str.append( QString("Number of Mode S messages received: %1\n").arg(a.messages));
    str.append( QString("+++++++++++++++++++++++++++++++++++++\n"));

    qDebug()<<str;
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QTcpSocket::RemoteHostClosedError)
        return;

    tcpServer.close();
}
