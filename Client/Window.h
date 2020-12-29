#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Window.h"

#include "Client.h"
#include "ShareFileWindow.h"

struct TorrentDownloadStatus;

class Window : public QMainWindow
{
    Q_OBJECT

    Client* client;
    ShareFileWindow* shareFileWindow;
public:
    Window(QWidget *parent = Q_NULLPTR);

private:
    Ui::WindowClass ui;
protected slots:
    void torrentStatusUpdated(const std::string torrentId, const TorrentDownloadStatus* status);
};
