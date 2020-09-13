#include <QDebug>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QObject>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("TCP Client");
    ui->ipEdit->setPlaceholderText("IP");
    ui->portEdit->setPlaceholderText("PORT");
    ui->textEdit->setPlaceholderText("Editor");
    host_ = "192.168.1.4";
    port_ = 8800;
    ui->ipEdit->setText(host_);
    ui->portEdit->setText(QString::number(port_));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter: {
            ui->textBrowser->insertPlainText("Key_Enter | Key_Return\n");
            break;
        }
        }
    }
}


void MainWindow::on_ipEdit_returnPressed()
{
    host_ =  ui->ipEdit->text();
}

void MainWindow::on_portEdit_returnPressed()
{
    port_ = ui->portEdit->text().toUInt();
}

void MainWindow::on_ipEdit_editingFinished()
{
    host_ = ui->ipEdit->text();
}

void MainWindow::on_portEdit_editingFinished()
{
    port_ = ui->portEdit->text().toUInt();
}

void MainWindow::on_sendBtn_clicked()
{

}

void MainWindow::on_newSocketBtn_clicked()
{
    QTcpSocket *socket = new QTcpSocket(this);
    connect(socket, static_cast<void (QTcpSocket:: *)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &MainWindow::onTcpError);
    ClientDialog *dialog = new ClientDialog(this, socket, host_, port_);
    connect(dialog, &QDialog::finished, dialog, &QObject::deleteLater);
    connect(dialog, &ClientDialog::debugInfo, this, &MainWindow::onDebugInfo);
    dialog->show();
}

void MainWindow::onTcpConnected()
{
    QTcpSocket *socket = reinterpret_cast<ClientDialog*>(sender())->getSocket();
    socket->waitForConnected();
    ui->textBrowser->insertPlainText(QString("Tcp connected Local Socket: %1:%2\n")
                                     .arg(socket->localAddress().toString()).arg(socket->localPort()));
}

void MainWindow::onTcpDisconnected()
{
    QTcpSocket *socket = reinterpret_cast<ClientDialog*>(sender())->getSocket();
    ui->textBrowser->insertPlainText(QString("Tcp disconnected Local Socket: %1:%2\n")
                                     .arg(socket->localAddress().toString()).arg(socket->localPort()));
}

void MainWindow::onTcpReadyRead()
{

}

void MainWindow::onTcpError(QAbstractSocket::SocketError error)
{
    QString errInfo;
    switch (error) {
    case QTcpSocket::ConnectionRefusedError: errInfo = "ConnectionRefusedError"; break;
    case QTcpSocket::RemoteHostClosedError: errInfo = "RemoteHostClosedError"; break;
    case QTcpSocket::HostNotFoundError: errInfo = "HostNotFoundError"; break;
    }
    ui->textBrowser->insertPlainText(QString("Error code: %1\nError info: %2\n").arg(error).arg(errInfo));
}

void MainWindow::onDebugInfo(QString info)
{
    ui->textBrowser->insertPlainText(info);
}
