/**
 * @file netzwerk.h Header file for the NetzWerk class
 * @author SitingWu
 **/
#ifndef NETZWERK_H
#define NETZWERK_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QDataStream>
#include <chrono>
#include <future>
#include<iostream>

/**
 *  @brief The NetzWerk class A simple TCP client
 */

class NetzWerk: public QObject {
     Q_OBJECT

 public:
     /**
      *  @brief NetzWerk Constructor
      *
      *  Sets the byte order of _InOutStream to BigEndian
      *
      *  @param[in] parent QObject acting as parent. Defaults to nullptr.
      **/
     explicit NetzWerk(QObject *parent = nullptr);
     ~NetzWerk();
    /**
     * @brief avoid that ,the same Message send in Multiple time
     * @return true if answer is received , false if not.
     **/
    bool sendMsgAktiv();

public slots:
    /**
     *  @brief Connects to a server. The server address and port is given through
     *   the UI-Interface
     * @param[in] ports
     * @param[in] addr address of the Computer
     *  Verifies whether the port is in the given range [1024, 65535] and attempts
     *   connection to Host. Also enables slots for TCP-Socket events.
     **/
    void connectToServer(quint16 port, const QString& addr);

     /**
      *  @brief Disconnects from server
      *  Eventually results in "closeConnection" being called.
      **/
     void disconnectFromServer();

     /**
      *  @brief Enables connection to a server, e.g. allows sending messages.
      **/
     void enableConnection();
     /**
      *  @brief Closes the connection to a server. Makes sure that the interface
      *   cannot attempt to send messages.
      **/
     void closeConnection();

     /**
      *  @brief Disconnects the socket from the server.
      *
      *  Eventually results in "closeConnection" being called.
      **/
     void disconnected();
     /**
      *  @brief Handles errors occurring with the TCP-Socket connection.
      *
      *  Eventually results in "closeConnection" being called.
      **/
     void socketError();
     /**
      *  @brief Send a connection requests
      **/
     void VerbindungsAnfragen();
     /**
      * @brief send the game parameters
      * @param[in] field size from x
      * @param[in] field size from y
      * @param[in] number of rounds
      * @param[in] Beginner this Round
      **/
 void SpiedlFeldParameter(quint8 X_Feld,
                              quint8 Y_Feld,
                              quint8 Rundenzahl,
                              quint8 Beginnender);

 void GameStarted(quint8 Rundenzahl, quint8 Beginner);
 void Zug(quint8 X_Feld);
 void AnsOfAnfrage(quint8 Statuscode);
 void AnsOfZug(quint8 Statuscode);
 /**
  * @brief recive Message.
  *        check the available Bytes, Bytes must more than 1 Byte
  *        and check if the rest Byte are equivalent to the Bytelen
  * */
 void reciveMsg();
 private:
     /**
      *  @brief The Stream that is used for communication via the TCP-Socket socket.
      **/
     QDataStream _InOutStream;
     /**
      *  @brief Threshold that is used to notice a spam attack.
      **/
     const quint32 _CONNECTION_THRESHOLD;

     /**
      * @brief Checks, whether enough bytes can be read from the TCP-Socket. It is
      *  assumed that incoming data is always sent as a QString. If this method
      *  (resp. waitForReadyRead) encounters a problem, it returns false. In that
      *  case, it is recommended to close the connection without further actions
      *  (as the task unmistakably requires sending QStrings, which might has
      *  failed).
      *
      * According to the Qt-Doku, Streams have no ability to check for available
      *  bytes.
      * However, it must be ensured that enough bytes are available!
      * Additionally, peeking data from the socket requires adjusting the byte order,
      * reinterpret_cast from quint32* to char* is not sufficient!
      *
      * @return true if the QString can be read, false if not.
      **/
     bool _assertQStringLength();

     /**
      * @brief Waits until numOfBytes bytes can be read from the TCP connection.
      *  Blocks until it returns.
      *
      * @param[in] numOfBytes
      * @param[in] timeout timeout to be waited for numOfBytes
      * @return true if enough bytes are readable, false if not.
      **/
     bool _waitForReadyRead(
                     const quint16& numOfBytes,
                     const std::chrono::seconds &timeout=std::chrono::seconds(4));
     /**
      * @brief avoid that ,the same Message send in Multiple time
      * @return true if answer is received , false if not.
      **/
     bool _sendMsgAktiv;

     quint8 _iden;
     /**
      * @brief Client send a VerbindungsAnfragen from Client.
      * if Sevrce received the Anfragen,then the parameter will throught the Interface
      * of the Spielfeld and the function SpielFeldParameter will be call.
      * @return true if answer is received , false if not.
      **/

      signals:
     void Signal_0(quint8 Cmd);
     void Signal_2(quint8 Cmd,quint8 breit,quint8 laenge,quint8 _Rundenzahl,quint8 Beginner);
     void Signal_3(quint8 Cmd,quint8 _Rundenzahl,quint8 Beginner);
     void Signal_4(quint8 Cmd,quint8 x);
     void Signal_10(quint8 Cmd,quint8 statuscode);
     void Signal_11(quint8 Cmd,quint8 statuscode);

 };

 #endif // NETZWERK_H
