#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();    

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_ipEdit_returnPressed();

    void on_portEdit_returnPressed();

    void on_ipEdit_editingFinished();

    void on_portEdit_editingFinished();

    void on_sendBtn_clicked();

    void on_newSocketBtn_clicked();

    void onTcpConnected();

    void onTcpDisconnected();

    void onTcpReadyRead();

    void onTcpError(QAbstractSocket::SocketError error);

    void onDebugInfo(QString info);

private:
    Ui::MainWindow *ui;

    QString host_;

    quint16 port_;
};
#endif // MAINWINDOW_H
