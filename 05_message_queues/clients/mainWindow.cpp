#include "mainWindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <sys/sysinfo.h>   // for system info examples
#include <sstream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Client 1");

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    auto* layout = new QVBoxLayout(central);
    sendButton = new QPushButton("Send Data", this);
    logEdit = new QTextEdit(this);
    logEdit->setReadOnly(true);

    layout->addWidget(sendButton);
    layout->addWidget(logEdit);

    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendMessage);

    // open message queue for sending
    // mq = std::make_unique<MessageQueue>(queueName, O_WRONLY);
}

void MainWindow::onSendMessage() {
    std::string message = prepareData();
    //reopening each time bc otherwise would need a way to signal to client that queue is stale
    // when deleting and creating a new queue for example
    //client saves a stale pointer, writing to a queue that doesn't exist
    try {
        MessageQueue temp(queueName, O_WRONLY | O_NONBLOCK);
        temp.send(message);
        logEdit->append(QString::fromStdString("sent: " + message));
    } catch (std::exception& ex) {
        logEdit->append(QString("Failed to send: %1").arg(ex.what()));
        QMessageBox::warning(this, "Send Error", ex.what());
    }
}

// std::string MainWindow::prepareData() {
//     return "default message";
// }