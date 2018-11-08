#include "MainWindow.h"
#include <QApplication>

#include "Core.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Core core;
    core.init();

    return a.exec();
}
