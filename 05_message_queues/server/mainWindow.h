#pragma once
#include <QMainWindow>
#include <QTimer>
#include <memory>
#include <QPushButton>
#include <QTextEdit>
#include <QSpinBox>
#include "../mq_wrapper.h"

namespace Ui {class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow();

private slots:
    void onCreateQueue();
    void onStartPolling();
    void onStopPolling();
    void onCloseQueue();
    void pollQueue();

private:
    // Widgets
    QTextEdit* logEdit;
    QPushButton *createBtn, *startBtn, *stopBtn, *closeBtn;
    QSpinBox* intervalSpin;

    // Timer and queue
    QTimer* pollTimer;
    std::unique_ptr<MessageQueue> mq;
    const std::string queueName = "/mailslot_queue";
};