#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serverTCP = new server;
    connect(serverTCP, &server::SigReceivedMessage, this, &MainWindow::SlotSendTextBrowser);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSendTextBrowser(QString str)
{
    this->ui->textBrowser->append(str);
    serverTCP->SendToClient(str);

}

