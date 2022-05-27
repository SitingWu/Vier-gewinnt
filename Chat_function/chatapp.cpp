#include <thread>
#include <chrono>
#include <cassert>
#include <iostream>

#include "chatapp.h"
#include "ui_chatapp.h"

ChatApp::ChatApp(QWidget *parent) : QWidget(parent),
                                    _ui(new Ui::ChatApp),
                                    _CONNECTION_THRESHOLD(20000){

    _ui->setupUi(this);
    _InOutStream.setByteOrder(QDataStream::BigEndian);
}

ChatApp::~ChatApp() {
    delete _ui;
    _ui = nullptr;
}

void ChatApp::disconnectFromServer() {
    dynamic_cast<QTcpSocket*>(_InOutStream.device())->disconnectFromHost();
}

void ChatApp::enableConnection() {
    _ui->teLog->appendPlainText("Connected.");
    connect(QObject::sender(), SIGNAL(readyRead()), this, SLOT(processInput()));
    connect(_ui->leMessage, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
}

void ChatApp::disconnected() {
    _ui->teLog->appendPlainText("Disconnecting/Deleting Socket");

}

void ChatApp::socketError() {
    _ui->teLog->appendPlainText("Connection closed or could not connect.");

}

void ChatApp::closeConnection() {
    disconnect(_ui->leMessage, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
}

void ChatApp::sendMessage() {
    QString nickname = _ui->leNickname->text();
    QString message  = _ui->leMessage->text();

    _appendToLog(nickname, message);

    emit Nickname_send(nickname);
    emit Message_send(message);

    /* send nickname + message */
   // _InOutStream << nickname << message;


    _ui->leMessage->clear();
}

void Chatapp::Nickname_rec(std::vector<quint8> myvector)
{  QString nickname;
    int size = static_cast<int>(myvector.size());
    for(i=0;i<size;i++)
    {
        nickname+=Qsting::nummer(myvector[i]);

    }
    _Nickname=nick;

}

void Chatapp::Message_rec(std::vector<quint8> myvector)
{  QString message;
    int size = static_cast<int>(myvector.size());
    for(i=0;i<size;i++)
    {
        message+=Qsting::nummer(myvector[i]);

    }
    _appendToLog(_Nickname, message);

}



void ChatApp::_appendToLog(const QString& nickname, const QString& message) {
    _ui->teLog->appendPlainText("<" + nickname + "> " + message);
}

bool ChatApp::_assertQStringLength() {
    /* According to the Qt-Doku, Streams have no ability to check for available
     *  bytes. However, it must be ensured that enough bytes are available!
     *  Additionally, peeking data from the socket requires adjusting the byte
     *  order, reinterpret_cast from quint32* to char* is not sufficient!
     */

    // For reading the QString length
    if(_waitForReadyRead(4) == false)
        return false;

    char peeked[4];
    _InOutStream.device()->peek(peeked, 4);
    quint32 peekedLength = 0;
    for(char x : peeked) {
        peekedLength <<= 4;
        peekedLength  |= x;
    }
    // For the whole string
    return _waitForReadyRead(peekedLength + 4);
}

bool ChatApp::_waitForReadyRead( const quint16 &numOfBytes,
                                 const std::chrono::seconds &timeout ) {

    auto bytesAvailable = std::async( std::launch::async,
                  [&]() {
                    while(_InOutStream.device()->bytesAvailable() <  numOfBytes)
                        ;
                    return true;
                  });

    return bytesAvailable.wait_for(timeout) == std::future_status::ready;
}

