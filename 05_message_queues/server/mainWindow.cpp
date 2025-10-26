#include <QMainWindow>
#include <QApplication>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QSpinBox>
#include <QLabel>
#include <QDateTime>
#include <memory>
#include "mainWindow.h"
#include "../mq_wrapper.h"

MainWindow :: MainWindow(QWidget* parent)
        : QMainWindow(parent),
          pollTimer(new QTimer(this))
    {
        // Window settings
        setWindowTitle("Сервер POSIX Message Queue");
        resize(600, 400);

        // Central widget & layout
        QWidget* central = new QWidget(this);
        setCentralWidget(central);
        QVBoxLayout* mainLayout = new QVBoxLayout(central);

        // Log text edit
        logEdit = new QTextEdit(central);
        logEdit->setReadOnly(true);
        mainLayout->addWidget(logEdit);

        // Interval control
        QHBoxLayout* intervalLayout = new QHBoxLayout();
        QLabel* intervalLabel = new QLabel("Інтервал опитування (мс):", central);
        intervalSpin = new QSpinBox(central);
        intervalSpin->setRange(100, 10000);
        intervalSpin->setValue(1000);
        intervalLayout->addWidget(intervalLabel);
        intervalLayout->addWidget(intervalSpin);
        mainLayout->addLayout(intervalLayout);

        // Buttons
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        createBtn = new QPushButton("Створити чергу", central);
        startBtn  = new QPushButton("Почати опитування", central);
        stopBtn   = new QPushButton("Зупинити опитування", central);
        closeBtn  = new QPushButton("Закрити чергу", central);
        buttonLayout->addWidget(createBtn);
        buttonLayout->addWidget(startBtn);
        buttonLayout->addWidget(stopBtn);
        buttonLayout->addWidget(closeBtn);
        mainLayout->addLayout(buttonLayout);

        // Connect buttons
        connect(createBtn, &QPushButton::clicked, this, &MainWindow::onCreateQueue);
        connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartPolling);
        connect(stopBtn, &QPushButton::clicked, this, &MainWindow::onStopPolling);
        connect(closeBtn, &QPushButton::clicked, this, &MainWindow::onCloseQueue);

        // Connect timer
        connect(pollTimer, &QTimer::timeout, this, &MainWindow::pollQueue);
    }

MainWindow :: ~MainWindow() {
        if (mq) {
            mq->closeOnly();
            MessageQueue::unlinkQueue(queueName);
        }
    }

void MainWindow :: onCreateQueue() {
        try {
            struct mq_attr attr;
            attr.mq_flags = 0; //default behaviour
            attr.mq_maxmsg = 10; //max msg in queue
            attr.mq_msgsize = 8192; //max bytes in queue
            attr.mq_curmsgs = 0;  //current num of msg (convention)
            //open for reading only (bc server), create if doesn't exist, make it non-blocking
            //read/write for everyone
            mq = std::make_unique<MessageQueue>(queueName, O_RDONLY | O_CREAT | O_NONBLOCK, 0666, &attr);
            logEdit->append(QString("[%1] Створено чергу %2")
                            .arg(QDateTime::currentDateTime().toString())
                            .arg(QString::fromStdString(queueName)));
        }
        catch (std::exception& ex) {
            logEdit->append(QString("Помилка створення черги: %1").arg(ex.what()));
        }
    }

void MainWindow :: onStartPolling() {
        int interval = intervalSpin->value();
        pollTimer->start(interval);
        logEdit->append("Почато опитування черги...");
    }

void MainWindow :: onStopPolling() {
        pollTimer->stop();
        logEdit->append("Зупинено опитування.");
    }

void MainWindow :: onCloseQueue() {
        if (mq) {
            mq->closeOnly();
            MessageQueue::unlinkQueue(queueName);
            mq.reset();
            logEdit->append("Чергу закрито та видалено.");
        }
    }

void MainWindow :: pollQueue() {
        if (!mq) return;
        try {
            std::string msg;
            while (!(msg = mq->tryReceive()).empty()) {
                logEdit->append(QString("[%1] Отримано: %2")
                                .arg(QDateTime::currentDateTime().toString())
                                .arg(QString::fromStdString(msg)));
            }
        } catch (...) {
            // ignore errors
        }
    }

// main function
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}

//for some reason this doesn't get read from .h
//so i just left it here for now
