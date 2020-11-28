#include <iostream>

#include "Window.h"

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    shareFileWindow = new ShareFileWindow(this);

    connect(ui.ShareNewFile, SIGNAL(clicked()), shareFileWindow, SLOT(show()));
}
