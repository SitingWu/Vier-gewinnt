#include <thread>
#include <chrono>
#include <cassert>
#include <iostream>

#include<vector>
#include <QIODevice>
#include "netzwerk.h"
#define TimeOut 10*1000 //10s


NetzWerk::NetzWerk(QObject *parent) : QObject(parent),//_ui(new Ui::NetzWerk),
                                     _CONNECTION_THRESHOLD(20000) {

    _InOutStream.setByteOrder(QDataStream::BigEndian);

}

NetzWerk::~NetzWerk() {

}


void NetzWerk::connectToServer(quint16 port, const QString& addr) {
    //quint16 port = _ui->lePort->text().toShort();

    /* Check if port is in range [1024, 65535].
     * port cannot be bigger than 65535 due to limited range of data type (quint16)
     * toShort() returns 0 if the conversion failed. However, we do not want to
     * connect to port 0 anyway.*/

    if(port < 1024) {
        qDebug()<<"Error: Invalid port number.\n";
        return ;
    }

    /* Assert whether a connection has already been established */
        auto socket = new QTcpSocket(this);

        //for GUI
        /* _ui->teLog->appendPlainText("Connecting to " + _ui->leHost->text() + ":" +
                                      _ui->lePort->text());*/

        qDebug() << "Connecting to port " << port << "\n";
        socket->connectToHost(addr, port);
        connect( socket, SIGNAL(connected()),
                 this, SLOT(enableConnection()) );
        connect( socket, SIGNAL(disconnected()),
                 this, SLOT(disconnected()) );
        connect( socket, SIGNAL(error(QAbstractSocket::SocketError)),
                 this, SLOT(socketError()) );
        connect( socket, SIGNAL(destroyed()),
                 this, SLOT(closeConnection()) );
    }

void NetzWerk::disconnectFromServer() {
    dynamic_cast<QTcpSocket*>(_InOutStream.device())->disconnectFromHost();
}

void NetzWerk::enableConnection() {

   assert(QObject::sender() != nullptr);
    qDebug() << "Connected \n";
    _InOutStream.setDevice(dynamic_cast<QTcpSocket*>(QObject::sender()));
    //When the Connetion seccess than should on Client side send a VerbingungsAnfagen to Server
    //if(/*  */)
    connect(QObject::sender(), SIGNAL(readyRead()), this, SLOT(VerbindungsAnfangen()));
}

void NetzWerk::disconnected() {
    _InOutStream.device()->deleteLater();
}

void NetzWerk::socketError() {
    _InOutStream.device()->deleteLater();
}

void NetzWerk::closeConnection() {
    _InOutStream.setDevice(nullptr);
}


// for QUI botton.enable
bool NetzWerk:: sendMsgAktiv(){
    return _sendMsgAktiv;
}

//Send Msg
//ask for connection
void NetzWerk::VerbindungsAnfragen(){
    _InOutStream.setDevice(dynamic_cast<QTcpSocket*>(QObject::sender()));
    _InOutStream <<0x0100;
    _iden=0x00;
    _sendMsgAktiv=false;
}

//SpielFeldParameter() soll vom Logik aufrufen
//wenn die Verbingungsanfangen bekommt wird,die Inputparameter wird vom Interface eingeben
void NetzWerk::SpiedlFeldParameter(quint8 X_Feld, //Feldgröße
                                    quint8 Y_Feld,
                                    quint8 Rundenzahl,
                                    quint8 Beginnender) { //0x00:Sever ;0X01: Client
                                    quint8 Cmd= 0x02;
                                    quint8 Bytelen =0x04;


      //send Nachricht
   _InOutStream << Cmd<< Bytelen<< X_Feld<< Y_Feld<< Rundenzahl<<Beginnender;
    _sendMsgAktiv=false;
}


void NetzWerk::GameStarted(quint8 Rundenzahl,
                           quint8 Beginner){
    quint8 Cmd= 0x03;
    quint8 Bytelen =0x02;
    //send Nachricht
  _InOutStream << Cmd<<Bytelen<<Rundenzahl<<Beginner;
    _sendMsgAktiv=false;

}

void NetzWerk::Zug(quint8 X_Feld){
    quint8 Cmd= 0x04;
    quint8 Bytelen =0x01;
    _InOutStream<< Cmd<<Bytelen <<X_Feld;
    _sendMsgAktiv=false;

}

void NetzWerk::AnsOfAnfrage(quint8 Statuscode){

    quint8 Cmd= 0x10;
    quint8 Bytelen =0x01;
    _InOutStream<<Cmd<<Bytelen <<Statuscode;
    _sendMsgAktiv=false;
}

void NetzWerk::AnsOfZug(quint8 Statuscode){
    quint8 Cmd= 0x11;
    quint8 Bytelen =0x01;
    _InOutStream<< Cmd<<Bytelen <<Statuscode;
    _sendMsgAktiv=false;
}

//Recive Msg
void NetzWerk::reciveMsg() {
    _InOutStream.startTransaction();//restorable point

    quint8 Cmd;
    quint8 ByteLen;
    std::vector<quint8> myvector;


      //Blocks until new data is available for reading :TRUE, or until TimeOut passed : FALSE
     if(!dynamic_cast<QTcpSocket*>(_InOutStream.device())->waitForReadyRead(TimeOut)){
            return;//TimeOut or Byte noch nicht komplet empfangen
     }

        // if the avilable byte is small than 2 then aborts a read transaction
        if(_InOutStream.device()->bytesAvailable() <2||_InOutStream.device()->bytesAvailable() >6)
        {
            //Nachricht unvollständig oder zu viele Nachricht
            //Nochmal data auslesen
            return;
        }
        _InOutStream >> Cmd>>ByteLen;

       for(int i=0;i<ByteLen;i++){
           quint8 tmp;
           _InOutStream>> tmp;
          myvector.push_back(tmp);
         }
       if(!_InOutStream.commitTransaction())
       {
           return;//Wait for more data
        }else{//erfolgreich ausgelesen
           quint8 x;
           quint8 y;
           quint8 rundennummer ;
           quint8 Beginn;
           quint8 x_coor;
           quint8 status;
           quint8 code;
           switch(Cmd) {
           case 0x01:
              // connect(this,&NetzWerk::Singal_0,/*Logicptr*/,&Logic::net_slot_verbindungsafrage);
               emit Signal_0(Cmd);
               qDebug()<< "Verbindungsanfrage \n";
               break;
           case 0x02:
              x=myvector[0];
              y=myvector[1];
              rundennummer =myvector[2];
              Beginn=myvector[3];
             //  connect(this,&NetzWerk::Singal_2,/*Logicptr*/,&Logic::net_slot_spielfeld_parameter);
               emit Signal_2(Cmd,x,y,rundennummer,Beginn);
               qDebug() << "Aushandeln der Spielfeldparameter;\n X : "<<x<<"Y : "<<y<<"Rundenzahl : "<<rundennummer<<"Beginner : "<<Beginn<<"\n";
               break;
           case 0x03:
                rundennummer =myvector[0];
               Beginn=myvector[1];
              // connect(this,&NetzWerk::Singal_3,/*Logicptr*/,&Logic::net_slot_spielfeld_parameter);
               emit Signal_3(Cmd,rundennummer,Beginn);
               qDebug()<<"Anforderung Rundennummer\n";
               break;
           case 0x04:
               x_coor =myvector[2];
            //   connect(this,&NetzWerk::Singal_4,/*Logicptr*/,&Logic::net_slot_zug);
               emit Signal_4(Cmd,x_coor);
               qDebug()<< "Zug :"<<x_coor<<"\n";
               break;
           case 0x10:
               status=myvector[0];
            //   connect(this,&NetzWerk::Singal_10,/*Logicptr*/,&Logic::net_slot_antwort_auf_anfrage);
               emit Signal_10(Cmd,status);
               qDebug()<<"Antwort auf Anfrage\n Statuscode:"<<status<<"\n";
               break;
           case 0x11:
              code=myvector[0];
            //   connect(this,&NetzWerk::Singal_11,/*Logicptr*/,&Logic::net_slot_antwort_auf_zug);
               emit Signal_11(Cmd,code);
               qDebug()<<"Antwort auf Zug \n Statuscode : "<<code<<"\n";
               break;
           default://die byte vom socket noch mal auslesen
               qDebug()<< "Cmd ist nicht definiert\n";
               return;
           }
           myvector.clear();
           _sendMsgAktiv=true;
       }
}



bool NetzWerk::_assertQStringLength() {
    /* According to the Qt-Doku, Streams have no ability to check for available
     *  bytes. However, it must be ensured that enough bytes are available!
     *  Additionally, peeking data from the socket requires adjusting the byte
     *  order, reinterpret_cast from quint32* to char* is not sufficient!
     */

    // For reading the QString length
   if(_waitForReadyRead(2) == false)
        return false;

    char peeked[2];
    _InOutStream.device()->peek(peeked, 2);
    quint32 peekedLength = 0;
    for(char x : peeked) {
        peekedLength <<= 2;
        peekedLength  |= x;//peekedLength = peekedLength || x
    }
    // For the whole string
    return _waitForReadyRead(peekedLength + 2);
}

 bool NetzWerk:: _waitForReadyRead( const quint16 &numOfBytes,
                                         const std::chrono::seconds &timeout ) {

            auto bytesAvailable = std::async( std::launch::async,
                          [&]() {
                            while(_InOutStream.device()->bytesAvailable() <numOfBytes)
                                ;
                            return true;
                          });

            return bytesAvailable.wait_for(timeout) == std::future_status::ready;
 }


