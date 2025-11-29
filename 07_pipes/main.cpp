#include <QApplication>
#include "server.h"
#include "client.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QStringList args = app.arguments();

    if (args.contains("--client1")) {
        ClientWindow client(1);
        client.show();
        return app.exec();
    } 
    else if (args.contains("--client2")) {
        ClientWindow client(2);
        client.show();
        return app.exec();
    } 
    else {
        ServerWindow server;
        
        QObject::connect(&server, &ServerWindow::dataReceived, 
                         &server, &ServerWindow::updateLog);
        
        server.show();
        return app.exec();
    }
}