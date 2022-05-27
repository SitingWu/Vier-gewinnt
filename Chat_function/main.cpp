#include <QApplication>
#include "chatapp.h"
#include <netzwerk.h>

int main(int argc, char *argv[])
{
    QApplication qapp(argc, argv);
    ChatApp window;
    window.show();
    
    return qapp.exec();
    connect(netzwerk,&netzwerk::Signalconnected(),Chatapp,&Chatapp::enableConnection());
    connect(Chatapp,&Chatapp::Nickname_send(),netzwerk,&Nickname_recive());
    connect(Chatapp,&Chatapp::Message_send(),netzwerk,&netzwerk::Message_recive());


    connect(Netzwerk,&Netzwerk::Signal_81(),Chatapp,&Chatapp::Nickname_rec());
    connect(Netzwert,&Netzwerk::Signal_82(),Chatapp,&Chatapp::Message_rec());
}


//connect(netzwerk,&netzwerk::Signalconnected(),Chatapp,&Chatapp::enableConnection());
//connect(Chatapp,&Chatapp::Nickname_send(),netzwerk,&Nickname_recive());
//connect(Chatapp,&Chatapp::Message_send(),netzwerk,&netzwerk::Message_recive());


//connect(Netzwerk,&Netzwerk::Signal_81(),Chatapp,&Chatapp::Nickname_rec());
//connect(Netzwert;&Netzwerk::Signal_82(),Chatapp,&Chatapp::Message_rec());
