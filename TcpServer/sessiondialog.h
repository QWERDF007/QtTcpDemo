#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include <QTcpSocket>
#include "visiontcppackage.h"

namespace Ui {
class SessionDialog;
}

class SessionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SessionDialog(QWidget *parent = nullptr, QTcpSocket *socket=nullptr);
    ~SessionDialog();

protected:
    void keyPressEvent(QKeyEvent *) override;

signals:
    void debugInfo(QString);

private slots:
    void on_closeBtn_clicked();

    void onTcpStateChanged(QAbstractSocket::SocketState);

    void onTcpReadyRead();

    void on_textBrowser_cursorPositionChanged();

private:
    Ui::SessionDialog *ui;

    QTcpSocket *_socket = nullptr;

    VisionTcpPackage _package;

    int _parseState = 0;

    size_t _current = 0;

    size_t _length = 0;

    std::vector<uint8_t> _buffer;

    void read(const char *data, size_t size);
};

#endif // SESSIONDIALOG_H
