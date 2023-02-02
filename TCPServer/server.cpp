#include "server.h"

server::server()
{
        if (this->listen(QHostAddress::Any, 2323)) {// первый аргумент - слушать всех, второй указывает порт
            qDebug()<<"start";
        } else {
            qDebug()<<"error";
        }
}

void server::SendToClient(QString str)
{
    Data.clear();
    str += "server";
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out<< quint16(0); // записываем в первые два байта 0, чтобы зарезервировать место в кадре посылки
    out<<str; // отслыаемая строка прописывается через датастрим в битовый массив Data, для исключения искажений и реализоации независимости от платформы

    out.device()->seek(0); // переходим на 0ую позицию в посылке, чтобы перезаписать размер посылки. Эти два байта зарезервирвовали выше
    out<< quint16(Data.size() - sizeof(quint16));
    for (int i = 0; i < Sockets.size() ; i++ ) { // проходим по вектору и записываем в каждый клиент сообщение
        Sockets.at(i)->write(Data);
    }
}

void server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor); // дескриптор - не отрицательное число которое идентифицирует поток ввода вывода
    connect(socket, &QTcpSocket::readyRead, this, &server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    Sockets.push_back(socket);
    qDebug()<<"client connected" << socketDescriptor;
}

void server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();// определяем кто вызвал слот
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_9); // указываем какую версию использовать
      QString str;
    if (in.status() == QDataStream::Ok) {
        qDebug()<<"read...";


        while(1){
            if (nextBlockSize == 0) { // определяем известно ли нам сколько должно прийти байт. Это содержится в начале посылки первые 2 байта
                qDebug()<<"nextBlockSize = 0";
                if (socket->bytesAvailable() < 2) { //здесь контролируем что пришло миниум 2 байта, если нет то ждем когда придут первые 2 байта, чтобы узнать размер посылки
                    qDebug()<<"Data < 2";
                    break;
                }
                in>>nextBlockSize; // если пришли 2 байта, то записываем размер послыки в переменную
                qDebug()<<"nextBlockSize="<<nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize){// сравниваем сколько пришло и сколько нужно байт
                qDebug()<<"Data not full";
                break;
            }

            // дальше ведется запись в текст браузер

            in>>str;
            nextBlockSize = 0;
            qDebug()<<str;
            emit SigReceivedMessage(str);
            SendToClient(str);
            break;
        }
    } else {
        qDebug()<<"DataStream error";

    }
}
