#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Image Studio");

    MainWindow mainWindow;
    mainWindow.setWindowTitle(QObject::tr("Image Studio"));
    mainWindow.resize(900, 650);
    mainWindow.show();

    return app.exec();
}