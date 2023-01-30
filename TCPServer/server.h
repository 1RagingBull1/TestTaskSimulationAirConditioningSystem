#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>

class server: public QTcpServer
{
    Q_OBJECT
public:
    server();
    QTcpSocket *socket;

private:
    QVector <QTcpSocket*> Sockets; // вектор хранения всех сокетов
    QByteArray Data;
    void SendToClient(QString str); // функция передачи данных клиенту
    quint16 nextBlockSize = 0;
public slots:
    void incomingConnection(qintptr socketDescriptor) override;
    void slotReadyRead();
};

#endif // SERVER_H
