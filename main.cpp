#include <QApplication>
#include "tout.h"



int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    tout mainWindow;
    mainWindow.show();

    return app.exec();
}

