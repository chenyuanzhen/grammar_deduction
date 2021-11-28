#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QDebug>
#include <iostream>
#include "grammar.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
