#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include <QTimerEvent>
#include <QEvent>
#include <QTcpSocket>
#include "visiontcppackage.h"

namespace Ui {
class ClientDialog;
}

class ClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDialog(QWidget *parent = nullptr, QTcpSocket *socket=nullptr, QString host="localhost", quint16 port=8800);
    ~ClientDialog();

    QTcpSocket * getSocket();


protected:
    void keyPressEvent(QKeyEvent *) override;

    void timerEvent(QTimerEvent *) override;

signals:
    void connected();

    void disconnected();

    void debugInfo(QString);

private slots:
    void on_connectBtn_clicked();

    void on_disconnectBtn_clicked();

    void onTcpConnected();

    void onTcpDisconnected();

    void onTcpError(QAbstractSocket::SocketError error);

    void onTcpStateChanged(QAbstractSocket::SocketState state);

    void on_textBrowser_cursorPositionChanged();

    void on_fileBtn_clicked();

    void on_sendBtn_clicked();

private:
    Ui::ClientDialog *ui;

    QTcpSocket *socket_ = nullptr;

    QString host_;

    quint16 port_;

    int reconnectTimerId_;

    int reconnectTimes_ = 0;

    bool isTerminated_ = false;

    void write(VisionTcpPackage &package);
};

#endif // CLIENTDIALOG_H
