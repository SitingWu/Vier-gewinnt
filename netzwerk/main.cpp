#include "netzwerk.h"
#include <iostream>
#include <QObject>

int main(int argc, char *argv[])
{   const QString& addr= "127.0.0.1";
    quint16 port=27929;
    NetzWerk test;
    qDebug()<<"Adress : "<<addr;
    qDebug()<< "\n Port :"<<port;
    test.connectToServer(port,addr);

   return 1;
}
