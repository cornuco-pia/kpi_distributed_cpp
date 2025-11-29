#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QScreen>
#include <QApplication> 
#include <QStyle>
#include <QNetworkInterface>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include "common.h"

class ClientWindow : public QWidget {
    Q_OBJECT

public:
    ClientWindow(int id, QWidget *parent = nullptr) : QWidget(parent), clientId(id) {
        setWindowTitle(QString("Client %1").arg(clientId));
        resize(300, 150);

        auto *layout = new QVBoxLayout(this);
        auto *btn = new QPushButton("Send System Info to Server", this);
        layout->addWidget(btn);

        connect(btn, &QPushButton::clicked, this, &ClientWindow::sendData);
    }

private:
    int clientId;

    void sendData() {
        //O_NONBLOCK instead of O_WRONLY for non-blocking
        int fd = open(PIPE_NAME, O_WRONLY | O_NONBLOCK);
        
        if (fd == -1) {
            QString errorMsg;
            if (errno == ENOENT) {
                errorMsg = "Server pipe does not exist.\n(Is the server app running?)";
            } else if (errno == ENXIO) {
                errorMsg = "Server is not listening.\n(The server app is running but might be busy or restarting)";
            } else {
                errorMsg = QString("Connection error: %1").arg(strerror(errno));
            }
            
            QMessageBox::critical(this, "Connection Error", errorMsg);
            return;
        }

        //good practice
        int flags = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

        if (clientId == 1) {
            sendPacket(fd, "CPU Cores", std::thread::hardware_concurrency(), "threads");

            bool netUp = false;
            auto interfaces = QNetworkInterface::allInterfaces();
            for(const auto &iface : interfaces) {
                if(iface.flags().testFlag(QNetworkInterface::IsUp) && 
                   !iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
                    netUp = true;
                    break;
                }
            }
            sendPacket(fd, "Network Status", netUp ? 1.0 : 0.0, netUp ? "(Connected)" : "(Disconnected)");

            QScreen *screen = QGuiApplication::primaryScreen();
            sendPacket(fd, "Screen Width", screen->geometry().width(), "pixels");

        } else {
            sendPacket(fd, "App Font Size", QApplication::font().pointSize(), "points");

            int border = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
            sendPacket(fd, "Window Border", border, "pixels");

            QScreen *screen = QGuiApplication::primaryScreen();
            sendPacket(fd, "Color Depth", screen->depth(), "bits");
        }

        ::close(fd); 
        QMessageBox::information(this, "Success", "Data sent to server!");
    }

    void sendPacket(int fd, const char* name, double value, const char* desc) {
        MessagePacket p;
        p.clientId = clientId;
        strncpy(p.name, name, sizeof(p.name));
        p.value = value;
        strncpy(p.description, desc, sizeof(p.description));
        
        write(fd, &p, sizeof(MessagePacket));
    }
};

#endif