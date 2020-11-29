#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Window.h"
#include "ShareFileWindow.h"

class Window : public QMainWindow
{
    Q_OBJECT

    ShareFileWindow* shareFileWindow;
public:
    Window(QWidget *parent = Q_NULLPTR);

private:
    Ui::WindowClass ui;
};
