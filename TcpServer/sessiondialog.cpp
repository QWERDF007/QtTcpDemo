#include <QScrollBar>
#include <QHostAddress>
#include <opencv2/highgui.hpp>
#include "sessiondialog.h"
#include "ui_sessiondialog.h"

SessionDialog::SessionDialog(QWidget *parent, QTcpSocket *socket) :
    QDialog(parent),
    ui(new Ui::SessionDialog), _socket(socket), _package(-1)
{
    ui->setupUi(this);
    if (nullptr != _socket) {
        ui->ipEdit->setText(_socket->peerAddress().toString());
        ui->portEdit->setText(QString::number(_socket->peerPort()));
        connect(_socket, &QTcpSocket::stateChanged, this, &SessionDialog::onTcpStateChanged);
        connect(_socket, &QTcpSocket::readyRead, this, &SessionDialog::onTcpReadyRead);
    }
}

SessionDialog::~SessionDialog()
{
    qDebug() << "~SessionDialog" << ui->ipEdit->text() << ui->portEdit->text();
    delete ui;

}

void SessionDialog::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter: {
            ui->textBrowser->insertPlainText(QString("Send Message: \n"));
            break;
        }
        }
    }
}

void SessionDialog::on_closeBtn_clicked()
{
    if (nullptr != _socket)
        _socket->close();
    done(QDialog::Accepted);
}

void SessionDialog::onTcpStateChanged(QAbstractSocket::SocketState state)
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
        done(QDialog::Accepted);
        break;
    }
    default: stateInfo = "NoDefine"; break;
    }
    QString info = QString("Tcp State Changed: %1\nState Info: %2\n").arg(state).arg(stateInfo);
    emit debugInfo(info);
}

void SessionDialog::onTcpReadyRead()
{
    qDebug() << "onTcpReadyRead";
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());
    QByteArray bytes = socket->readAll();
    _current = 0;
    _length =  bytes.size();
    int rest = _length;
    qDebug() << "all:" << rest;
    const char *data = bytes.data();
    while (rest > 0) {
        switch (_parseState) {
        case 0: {
            _package.clear();
            _parseState = 1;
            break;
        }
        case 1: {
            qDebug() << "onTcpReadyRead request id";
            int numToRead = std::min<int>(VisionTcpPackage::S_LONG - _buffer.size(), rest);
            if (numToRead > 0) {
                read(data, numToRead);
                rest -= numToRead;
            }
            if (_buffer.size() >= VisionTcpPackage::S_LONG) {
                _parseState = 2;
                std::memcpy(reinterpret_cast<uint8_t*>(&_package.requestId), _buffer.data(), VisionTcpPackage::S_LONG);
                _buffer.clear();
                ui->textBrowser->insertPlainText(QString("request id: %1\n").arg(_package.requestId));
            }
            qDebug() << rest;
            break;
        }
        case 2: {
            qDebug() << "onTcpReadyRead header size";
            int numToRead = std::min<int>(VisionTcpPackage::U_LONG - _buffer.size(), rest);
            if (numToRead > 0) {
                read(data, numToRead);
                rest -= numToRead;
            }
            if (_buffer.size() >= VisionTcpPackage::U_LONG) {
                _parseState = 3;
                std::memcpy(reinterpret_cast<uint8_t*>(&_package.headerSize), _buffer.data(), VisionTcpPackage::U_LONG);
                _buffer.clear();
                ui->textBrowser->insertPlainText(QString("header size: %1\n").arg(_package.headerSize));
            }
            qDebug() << rest;
            break;
        }
        case 3: {
            qDebug() << "onTcpReadyRead data size";
            int numToRead = std::min<int>(VisionTcpPackage::U_LONG - _buffer.size(), rest);
            if (numToRead > 0) {
                read(data, numToRead);
                rest -= numToRead;
            }
            if (_buffer.size() >= VisionTcpPackage::U_LONG) {
                _parseState = 4;
                std::memcpy(reinterpret_cast<uint8_t*>(&_package.bufferSize), _buffer.data(), VisionTcpPackage::U_LONG);
                _buffer.clear();
                ui->textBrowser->insertPlainText(QString("data size: %1\n").arg(_package.bufferSize));
            }
            qDebug() << rest;
            break;
        }
        case 4: {
            qDebug() << "onTcpReadyRead header";
            int numToRead = std::min<int>(_package.headerSize - _buffer.size(), rest);
            if (numToRead > 0) {
                read(data, numToRead);
                rest -= numToRead;
            }
            if (_buffer.size() >= _package.headerSize) {
                _parseState = 5;
                _package.header = std::string(_buffer.data(), _buffer.data() + _package.headerSize);
                _buffer.clear();
                ui->textBrowser->insertPlainText(QString("header: %1\n").arg(_package.header.c_str()));
            }
            qDebug() << rest;
            break;
        }
        case 5: {
            qDebug() << "onTcpReadyRead data";
            try {
                int numToRead = std::min<int>(_package.bufferSize - _buffer.size(), rest);
                if (numToRead > 0) {
                    read(data, numToRead);
                    rest -= numToRead;
                }
                if (_buffer.size() >= _package.bufferSize) {
                    _parseState = 0;
                    _package.buffer.assign(_buffer.data(), _buffer.data() + _package.bufferSize);
                    cv::Mat image = cv::imdecode(_package.buffer, cv::IMREAD_COLOR);
                    if (!image.empty()) {
                        cv::imshow("image", image);
                    } else {
                        throw std::runtime_error("image empty!!!");
                    }
                    _buffer.clear();
                }
            } catch (std::exception &e) {
                qDebug() << e.what();
            }

            qDebug() << rest;
            break;
        }
        default: break;
        }
    }
}

void SessionDialog::on_textBrowser_cursorPositionChanged()
{
    QScrollBar *sb = ui->textBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void SessionDialog::read(const char *data, size_t size)
{
    size_t end = _current + size;
    if (end > _length) {
        qDebug() << "current: " << _current << "size: " << size << "length" << _length;
        throw std::runtime_error("buffer read out of length");
    }
    _buffer.insert(_buffer.end(), data + _current, data + end);
    _current = end;
}
