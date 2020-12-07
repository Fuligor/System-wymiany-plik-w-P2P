#include <iostream>

#include "Window.h"

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    client = Client::getInstance();
    shareFileWindow = new ShareFileWindow(ui.MainWidget, this);

    connect(ui.ShareNewFile, SIGNAL(clicked()), shareFileWindow, SLOT(show()));
}
