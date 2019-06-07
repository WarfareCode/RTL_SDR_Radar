#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&tcpServer, SIGNAL(newConnection()),
            this, SLOT(acceptConnection()));
}

MainWindow::~MainWindow()
{
    tcpServer.close();
    delete ui;
}

void MainWindow::acceptConnection()
{
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

    tcpServerConnection->close();
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QTcpSocket::RemoteHostClosedError)
        return;

    tcpServer.close();
}
