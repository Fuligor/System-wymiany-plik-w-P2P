#include "Window.h"
#include <QtWidgets/QApplication>
#include <QString>

#include "Client.h"

#include "TorrentDownloader.h"
#include "TorrentFile.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //TorrentDownloader downloader("Client.exe.torrent");

    Window w;
    w.show();
    return a.exec();
}
