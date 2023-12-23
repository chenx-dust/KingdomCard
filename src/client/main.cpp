#include "client/include/client.h"
#include "server/server.h"
#include "client/include/start.h"

#include <QApplication>
#include <QtConcurrent/QtConcurrent>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ServerWindow ServerWindow;
    ServerWindow.setWindowTitle("Server");
    ServerWindow.show();
//
//
    ClientWindow ClientWindow;
    ClientWindow.setWindowTitle("Client");
    ClientWindow.hide();

    StartWindow StartWindow;
    StartWindow.setWindowTitle("Start");
    StartWindow.show();

//     Close
    QObject::connect(&a, &QApplication::lastWindowClosed, &a, &QApplication::quit);

    return a.exec();
}
