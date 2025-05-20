#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QString>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);
    QApplication a(argc, argv);
    QIcon app_icon(":/icons/src/images/favicon.ico");
    a.setWindowIcon(app_icon);

    QFile style_file(":/style/src/styles/dark_mainwindow.qss");
    if (style_file.open(QFile::ReadOnly | QFile::Text)) {
        QString style =style_file.readAll();
        a.setStyleSheet(style);
        style_file.close();
    }
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
