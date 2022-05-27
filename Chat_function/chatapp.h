/**
 * @file chatapp.h Header file for the ChatApp class
 * @author Tobias Fuchs
 **/

#ifndef CHATAPP_H
#define CHATAPP_H

#include <QWidget>
#include <QtNetwork/QTcpSocket>
#include <QDataStream>
#include <chrono>
#include <future>

namespace Ui {
    class ChatApp;
}

/**
 *  @brief The ChatApp class A simple chat client
 */
class ChatApp : public QWidget {
    Q_OBJECT

public:
    /**
     *  @brief ChatApp Constructor
     *
     *  Sets the byte order of _InOutStream to BigEndian
     *
     *  @param[in] parent QObject acting as parent. Defaults to nullptr.
     **/
    explicit ChatApp(QWidget *parent = 0);
    ~ChatApp();

private slots:
    /**
     *  @brief Disconnects from server
     *
     *  Eventually results in "closeConnection" being called.
     **/
    void disconnectFromServer();
    /**
     * @brief Sends a chat message (nickname + message) over the network.
     *
     * Reads message and nickname from the GUI and sends {Nick Length, Nick, Msg
     *  Length, Msg} via QDataStream. Nick Length and Msg Length are sent in
     *  LittleEndian format, whereas Nick and Msg are sent in BigEndian format.
     **/
    void sendMessage();
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

    void Nickname_rec(std::vector<quint8> myvector);
     void Message_rec(std::vector<quint8> myvector);

private:
    /**
     *  @brief Pointer to the user interface
     **/
    Ui::ChatApp *_ui;
    /**
     *  @brief The Stream that is used for communication via the TCP-Socket socket.
     **/
    QDataStream _InOutStream;
    /**
     *  @brief Threshold that is used to notice a spam attack.
     **/
    const quint32 _CONNECTION_THRESHOLD;

    /**
     * @brief Appends the message to the GUI chat log.
     * @param[in] nickname
     * @param[in] message
     */
    void _appendToLog(const QString& nickname, const QString& message);
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
    QString _Nickname;

    signal:
    void Nickname_send(Qstring nickname);
    void Message_send(Qstring message);
};

#endif // CHATAPP_H
