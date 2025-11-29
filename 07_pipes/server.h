#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QApplication>
#include <thread>
#include <atomic>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "common.h"

class ServerWindow : public QWidget {
    Q_OBJECT

public:
    ServerWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Server: Named Pipe Monitor");
        resize(400, 300);

        auto *layout = new QVBoxLayout(this);
        logArea = new QTextEdit(this);
        logArea->setReadOnly(true);
        layout->addWidget(new QLabel("Waiting for Client Data...", this));
        layout->addWidget(logArea);

        // 0666 = rw permissions for everyone
        mkfifo(PIPE_NAME, 0666);

        running = true;
        pipeThread = std::thread(&ServerWindow::readPipeLoop, this);
    }

    ~ServerWindow() {
        running = false;
        // connect to pipe briefly to unblock the read loop so thread can exit
        int fd = open(PIPE_NAME, O_WRONLY | O_NONBLOCK);
        if(fd != -1) ::close(fd);
        
        if (pipeThread.joinable()) {
            pipeThread.join();
        }
        unlink(PIPE_NAME); //removes from tmp
    }

signals:
    void dataReceived(QString message);

public slots:
    void updateLog(QString message) {
        logArea->append(message);
    }

private:
    QTextEdit *logArea;
    std::thread pipeThread;
    std::atomic<bool> running;

    void readPipeLoop() {
        while (running) {
            int fd = open(PIPE_NAME, O_RDONLY);
            if (fd == -1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue; 
            }

            MessagePacket packet;
            ssize_t bytesRead;

            while ((bytesRead = read(fd, &packet, sizeof(MessagePacket))) > 0) {
                QString msg = QString("[%1] %2: %3 %4")
                              .arg(packet.clientId == 1 ? "Client 1" : "Client 2")
                              .arg(packet.name)
                              .arg(packet.value)
                              .arg(packet.description);
                
                emit dataReceived(msg);
            }

            ::close(fd);
        }
    }
};

#endif