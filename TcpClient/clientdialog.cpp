#include <QFileDialog>
#include <QScrollBar>
#include <QHostAddress>
#include <QDateTime>
#include <opencv2/highgui.hpp>

#include "clientdialog.h"
#include "ui_clientdialog.h"

ClientDialog::ClientDialog(QWidget *parent, QTcpSocket *socket, QString host, quint16 port) :
    QDialog(parent),
    ui(new Ui::ClientDialog), socket_(socket), host_(host), port_(port)
{
    ui->setupUi(this);
    ui->disconnectBtn->setEnabled(false);
    if (nullptr != socket_) {
        connect(socket_, &QTcpSocket::connected, this, &ClientDialog::onTcpConnected);
        connect(socket_, &QTcpSocket::disconnected, this, &ClientDialog::onTcpDisconnected);
        connect(socket_, &QTcpSocket::stateChanged, this, &ClientDialog::onTcpStateChanged);
    }

}

ClientDialog::~ClientDialog()
{
    delete ui;
}

QTcpSocket *ClientDialog::getSocket()
{
    return socket_;
}

void ClientDialog::write(VisionTcpPackage &package)
{
    if (socket_->state() == QTcpSocket::ConnectedState) {
        const char *data = reinterpret_cast<const char *>(&package.requestId);
        socket_->write(data, VisionTcpPackage::S_LONG);

        data = reinterpret_cast<const char *>(&package.headerSize);
        socket_->write(data, VisionTcpPackage::U_LONG);

        data = reinterpret_cast<const char *>(&package.bufferSize);
        socket_->write(data, VisionTcpPackage::U_LONG);

        socket_->write(package.header.c_str(), package.header.size());

        socket_->write(reinterpret_cast<const char *>(package.buffer.data()), package.bufferSize);
    }
}

void ClientDialog::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter: {
            if (socket_->state() ==  QTcpSocket::ConnectedState) {
            }
            break;
        }
        }
    }
}

void ClientDialog::timerEvent(QTimerEvent *event)
{
    int timerId = event->timerId();
    if (timerId == reconnectTimerId_) {
        if (reconnectTimes_ >= 3 || socket_->state() == QTcpSocket::ConnectedState) {
            killTimer(timerId);
            reconnectTimerId_ = 0;
            reconnectTimes_ = 0;
        } else {
            socket_->connectToHost(host_, port_);
            ++reconnectTimes_;
        }
    }
}

void ClientDialog::on_connectBtn_clicked()
{
    isTerminated_ = false;
    socket_->connectToHost(host_, port_);
}

void ClientDialog::on_disconnectBtn_clicked()
{
    isTerminated_ = true;
    if (nullptr != socket_) {
        socket_->close();
    }
}

void ClientDialog::onTcpConnected()
{
    QString info = QString("Connected to Host: %1:%2\n").arg(host_).arg(port_);
    ui->connectBtn->setEnabled(false);
    ui->disconnectBtn->setEnabled(true);
    ui->ipEdit->setText(socket_->localAddress().toString());
    ui->portEdit->setText(QString::number(socket_->localPort()));
    ui->textBrowser->insertPlainText(info);
    emit debugInfo(info);
}

void ClientDialog::onTcpDisconnected()
{
    QString info = QString("Disconnect from Host: %1:%2\n").arg(host_).arg(port_);
    ui->connectBtn->setEnabled(true);
    ui->disconnectBtn->setEnabled(false);
    ui->textBrowser->insertPlainText(info);
    if (socket_->state() == QTcpSocket::UnconnectedState && !isTerminated_) {
        ui->textBrowser->insertPlainText(QString("Try Reconnect\n"));
        reconnectTimerId_ = startTimer(3000);
    }
    emit debugInfo(info);
}

void ClientDialog::onTcpError(QAbstractSocket::SocketError error)
{
    ui->textBrowser->insertPlainText(QString("Error: %1\n").arg(error));
}

void ClientDialog::onTcpStateChanged(QAbstractSocket::SocketState state)
{
    QString stateInfo;
    switch (state) {
    case QTcpSocket::UnconnectedState:{
        stateInfo = "UnconnectedState";
        break;
    }
    case QTcpSocket::HostLookupState: {
        stateInfo = "HostLookupState"; break;
    }
    case QTcpSocket::ConnectingState: {
        stateInfo = "ConnectingState";
        break;
    }
    case QTcpSocket::ConnectedState: {
        stateInfo = "ConnectedState";
        break;
    }
    case QTcpSocket::BoundState: {
        stateInfo = "BoundState";
        break;
    }
    case QTcpSocket::ListeningState: {
        stateInfo = "ListeningState";
        break;
    }
    case QTcpSocket::ClosingState: {
        stateInfo = "ClosingState";
//        isTerminated = true;
        break;
    }
    default: stateInfo = "NoDefine"; break;
    }
//    QString info = QString("State Code: %1\nState Info: %2\n").arg(state).arg(stateInfo);
//    ui->textBrowser->insertPlainText(info);
//    emit debugInfo(info);
}

void ClientDialog::on_textBrowser_cursorPositionChanged()
{
    QScrollBar *sb = ui->textBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void ClientDialog::on_fileBtn_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this, "Open File", "", "Images (*.png *.jpg *.jpeg)");
    if (!filepath.isEmpty()) {
        std::string imagePath = filepath.toStdString();
        VisionTcpPackage package(QDateTime::currentDateTime().toSecsSinceEpoch());
        package.header = imagePath;
        package.headerSize = imagePath.size();
        cv::Mat image = cv::imread(imagePath);
        cv::imencode(imagePath.substr(imagePath.size() - 4, 4), image, package.buffer);
        package.bufferSize = package.buffer.size();
        ui->textBrowser->insertPlainText(QString("request id: %1\n").arg(package.requestId));
        ui->textBrowser->insertPlainText(QString("header size: %1\n").arg(package.headerSize));
        ui->textBrowser->insertPlainText(QString("data size: %1\n").arg(package.bufferSize));
        ui->textBrowser->insertPlainText(QString("header: %1\n").arg(package.header.c_str()));
        write(package);
    }
}

void ClientDialog::on_sendBtn_clicked()
{

}
