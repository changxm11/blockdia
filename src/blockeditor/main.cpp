#include "mainwindow.h"
#include <QCoreApplication>
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Oinosseus");
    QCoreApplication::setOrganizationDomain("https://github.com/Oinosseus/blockdia");
    QCoreApplication::setApplicationName("Block Editor");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
