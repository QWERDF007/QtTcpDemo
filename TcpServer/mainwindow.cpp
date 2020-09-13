#include <QScrollBar>
#include <QMessageBox>
#include <QNetworkInterface>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sessiondialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), server_(new QTcpServer(this))
{
    ui->setupUi(this);
    setWindowTitle("TCP Server");
    ui->ipEdit->setPlaceholderText("IP");
    ui->portEdit->setPlaceholderText("PORT");
    host_ = getLocalHostIPv4().toString();
    port_ = 8800;
    ui->ipEdit->setText(host_);
    ui->portEdit->setText(QString::number(port_));
    connect(server_, &QTcpServer::newConnection, this, &MainWindow::onNewConnection);
    connect(server_, &QTcpServer::acceptError, this, &MainWindow::onAcceptError);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QHostAddress MainWindow::getLocalHostIPv4()
{
    QHostAddress host;
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (QHostAddress address : QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            host = address;
    }
    return host;
}


void MainWindow::on_listenBtn_clicked()
{
    if (!isListening) {
        if (!server_->listen(QHostAddress::AnyIPv4, port_)) {
            ui->textBrowser->insertPlainText(QString("Can't Listen Port:%1").arg(port_));
            server_ = nullptr;
        } else {
            isListening = true;
            ui->textBrowser->insertPlainText(QString("Listening Port: %1\n").arg(port_));
            ui->listenBtn->setEnabled(false);
            ui->stopBtn->setEnabled(true);
        }
    }
}

void MainWindow::on_portEdit_editingFinished()
{

}

void MainWindow::on_portEdit_returnPressed()
{

}

void MainWindow::onNewConnection()
{
    QTcpSocket *socket = server_->nextPendingConnection();
    connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
    connect(socket, static_cast<void (QTcpSocket:: *)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &MainWindow::onTcpError);
    SessionDialog *dialog = new SessionDialog(this, socket);
    connect(dialog, &QDialog::finished, dialog, &QObject::deleteLater);
    connect(dialog, &SessionDialog::debugInfo, this, &MainWindow::onDebugInfo);
    dialog->show();
    ui->textBrowser->insertPlainText(QString("New connection:\n%1:%2\n").arg(socket->peerAddress().toString()).arg(socket->peerPort()));
}

void MainWindow::onTcpConnected()
{

}

void MainWindow::onTcpDisconnected()
{
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());
    ui->textBrowser->insertPlainText(QString("Disconnected from Client: %1:%2\n").arg(socket->peerAddress().toString()).arg(socket->peerPort()));
}

void MainWindow::onTcpReadyRead()
{

}

void MainWindow::onTcpError(QAbstractSocket::SocketError error)
{
    QString errinfo = getErrorInfo(error);
    ui->textBrowser->insertPlainText(QString("Tcp Error: %1\nError info: %2\n").arg(error).arg(errinfo));
}

void MainWindow::onAcceptError(QAbstractSocket::SocketError error)
{
    QString errinfo = getErrorInfo(error);
    ui->textBrowser->insertPlainText(QString("Accept Error: %1\nError info: %2\n").arg(error).arg(errinfo));
}

void MainWindow::on_sendBtn_clicked()
{

}

void MainWindow::on_stopBtn_clicked()
{
    if (nullptr != server_) {
        isListening = false;
        server_->close();
        ui->listenBtn->setEnabled(true);
        ui->stopBtn->setEnabled(false);
        ui->textBrowser->insertPlainText(QString("Stop Listening Port: %1\n").arg(port_));
    }
}

void MainWindow::onDebugInfo(QString info)
{
    ui->textBrowser->insertPlainText(info);
}

void MainWindow::on_textBrowser_cursorPositionChanged()
{
    QScrollBar *sb = ui->textBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
}

QString MainWindow::getErrorInfo(QAbstractSocket::SocketError error)
{
    QString errinfo;
    switch (error) {
    case QTcpSocket::ConnectionRefusedError: errinfo = "ConnectionRefusedError"; break;
    case QTcpSocket::RemoteHostClosedError: errinfo = "RemoteHostClosedError"; break;
    case QTcpSocket::HostNotFoundError: errinfo = "HostNotFoundError"; break;
    default: errinfo = "NoDefine"; break;
    }
    return errinfo;
}
