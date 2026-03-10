#include "mainwindow.h"
#include "boardwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Information Board");
    app.setApplicationVersion("1.0");

    MainWindow consoleWindow;
    BoardWindow boardWindow;

    consoleWindow.setBoardWindow(&boardWindow);

    consoleWindow.move(100, 100);
    boardWindow.move(700, 100);

    consoleWindow.show();
    boardWindow.show();

    return app.exec();
}
