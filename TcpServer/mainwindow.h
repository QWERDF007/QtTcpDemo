#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    /**
     * @brief 获取本机IP
     * @return 本机IP, 非127.0.0.1
     */
    QHostAddress getLocalHostIPv4();

private slots:
    void on_listenBtn_clicked();

    void on_portEdit_editingFinished();

    void on_portEdit_returnPressed();

    void onNewConnection();

    void onTcpConnected();

    void onTcpDisconnected();

    void onTcpReadyRead();

    void onTcpError(QAbstractSocket::SocketError error);

    void onAcceptError(QAbstractSocket::SocketError error);

    void on_sendBtn_clicked();

    void on_stopBtn_clicked();

    void onDebugInfo(QString info);

    void on_textBrowser_cursorPositionChanged();

private:
    Ui::MainWindow *ui;

    QString host_;

    quint16 port_;

    QTcpServer *server_ = nullptr;

    bool isListening = false;

    QString getErrorInfo(QAbstractSocket::SocketError error);
};
#endif // MAINWINDOW_H
