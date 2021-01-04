#include "ShareFileWindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QListView>
#include <QTreeView>
#include <QMessageBox>
#include <QFile>
#include <QStringListModel>

#include "FileSize.h"
#include "Client.h"
#include "TorrentManager.h"

SizeUnit getUnit(std::string u)
{
    if (u == "B")
    {
        return SizeUnit::B;
    }
    else if (u == "kB")
    {
        return SizeUnit::kB;
    }
    else if (u == "MB")
    {
        return SizeUnit::MB;
    }
    else
    {
        return SizeUnit::GB;
    }
}

ShareFileWindow::ShareFileWindow(QWidget* prevWindow, QWidget *parent)
    : QWidget(parent), prevWindow(prevWindow), fileDialog(new QFileDialog(this))
{
    ui.setupUi(this);
    hide();
    fileDialog->setModal(true);

    connect(ui.Cancel, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui.listFiles, SIGNAL(clicked()), fileDialog, SLOT(show()));
    connect(fileDialog, SIGNAL(accepted()), this, SLOT(fileSelected()));
    connect(ui.Share, SIGNAL(clicked()), this, SLOT(check()));
    connect(this, SIGNAL(checked()), this, SLOT(shareFile()));
    connect(this, SIGNAL(checked()), this, SLOT(hide()));

    QFile file("trackerSaves.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) { 
        QTextStream in(&file);
        while (!in.atEnd()) 
        { 
            ui.trackerURL->addItem(in.readLine()); 
        }
        file.close();
    }
}

ShareFileWindow::~ShareFileWindow()
{
}

void ShareFileWindow::init()
{
    ui.fileName->setText("");
}

//void ShareFileWindow::getFiles()
//{
//    fileDialog->show();
//}

void ShareFileWindow::fileSelected()
{
    ui.fileName->setText(fileDialog->selectedFiles()[0]);
    qDebug() << fileDialog->selectedFiles()[0];
}

void ShareFileWindow::shareFile()
{
    std::string unit = ui.sizeUnit->currentText().toStdString();
    FileSize unitSize = FileSize(ui.pieceSize->value(), getUnit(unit));

    QString trackerURL = ui.trackerURL->currentText();
    ui.trackerURL->addItem(trackerURL);

    QFile file("trackerSaves.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        
        for (int i = 0; i < ui.trackerURL->count(); i++)
        {
            out << ui.trackerURL->itemText(i) + "\n";
        }
        file.close();
    }

    Client::getInstance()->shareFile(ui.fileName->text().toStdString(), trackerURL.toStdString(), unitSize.to_int());
}

void ShareFileWindow::check()
{
    QRegularExpression re("^(?:[0-9]{1,3}.){3}[0-9]{1,3}:[0-9]{4}$");
    if (ui.fileName->text().isEmpty() || ui.trackerURL->currentText().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText(QString::fromStdWString(L"Przynajmniej jedno pole jest puste."));
        msgBox.setInformativeText(QString::fromStdWString(L"Uzupełnij puste pola, by kontynuować."));
        msgBox.exec();
    }
    if (!ui.trackerURL->currentText().contains(re))
    {
        QMessageBox msgBox;
        msgBox.setText(QString::fromStdWString(L"Podano błędny adres trackera."));
        msgBox.setInformativeText(QString::fromStdWString(L"Popraw adres trackera, by kontynuować."));
        msgBox.exec();
    }
    else
    {
        emit checked();
    }
}
