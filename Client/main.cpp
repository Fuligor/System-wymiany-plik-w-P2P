#include "Window.h"
#include <QtWidgets/QApplication>
#include <QString>

#include "Client.h"

#include "TorrentManager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QObject::connect(&a, SIGNAL(aboutToQuit()), &(TorrentManager::getInstance()), SLOT(clear()));

    Window w;
    w.show();
    return a.exec();
}
