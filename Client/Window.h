#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Window.h"

#include "Client.h"
#include "ShareFileWindow.h"
#include "DownloadFileWindow.h"

#include <map>
#include <vector>

struct TorrentDownloadStatus;

class Window : public QMainWindow
{
    Q_OBJECT

    Client* client;
    ShareFileWindow* shareFileWindow;
    DownloadFileWindow* downloadFileWindow;

    std::map <std::string, int> idToRow;
    std::vector <const TorrentDownloadStatus*> rowStatus;
public:
    Window(QWidget *parent = Q_NULLPTR);

private:
    Ui::WindowClass ui;
protected slots:
    void torrentStatusUpdated(const std::string torrentId, const TorrentDownloadStatus* status);
    void torrentPieceDownloaded(const TorrentDownloadStatus* status);
    void torrentPieceUploaded(const TorrentDownloadStatus* status);
    void torrentSpeedUpdated(const TorrentDownloadStatus* status);
    void torrentNewConnection(const TorrentDownloadStatus* status);
    void updateBottomBar(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void showWarning(std::string fileName);
    void deleteFile();
};
