#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <memory>
#include "../mq_wrapper.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onSendMessage();

private:
    QTextEdit* logEdit;
    QPushButton* sendButton;
    std::unique_ptr<MessageQueue> mq;
    const std::string queueName = "/mailslot_queue";

    virtual std::string prepareData() {return std::string {};};  // NEEDS TO BE IMPLEMENTED for each client separately
};

