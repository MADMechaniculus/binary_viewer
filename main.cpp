#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QMetaType>
#include <QVector>

int main(int argc, char *argv[])
{   
    qRegisterMetaType<QVector<uint32_t>>("QVector<uint32_t>");

    QFile file(":/dark/stylesheet.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    QApplication a(argc, argv);
    a.setStyleSheet(stream.readAll());

    MainWindow w;
    w.show();
    return a.exec();
}
