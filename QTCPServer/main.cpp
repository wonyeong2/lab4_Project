#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication aplct(argc, argv);
    MainWindow wind;
    wind.show();

    return aplct.exec();
}
