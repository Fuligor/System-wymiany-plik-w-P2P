#include "Window.h"
#include <QtWidgets/QApplication>
#include <QString>

#include "Client.h"

#include "TorrentFile.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::string name = "Client.exe";
    std::string adress = "192.168.1.119:1234";

    TorrentFile* file = new TorrentFile(name, adress, 4096);
    file->createFile();
    delete file;

    TrackerConnection connection("Client.exe.torrent");

    Window w;
    w.show();
    return a.exec();
}
