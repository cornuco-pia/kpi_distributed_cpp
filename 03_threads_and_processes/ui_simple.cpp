#include <iostream>
#include <thread>
#include <QMainWindow>
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>
#include <QThread>

const int MAX_THREADS = 5;
std::atomic<int> activeThreads{0};

class MyThread : public QThread {
    int id;
    bool ascending;

public:
    MyThread(int id, bool asc): id(id), ascending(asc) {}

protected:
    void run() override {
        qDebug() << "[Thread" << id << "] started " << (ascending? "ascending" : "descending");
    
    if (ascending) {
        for (int i = 1; i <= 100; i++){
            qDebug() << "[Thread" << id << "]" << i;
            QThread::msleep(1000);
        }
    } else {
        for (int i = 100; i >= 1; i--){
            qDebug() << "[Thread" << id << "]" << i;
            QThread::msleep(1000);
        }
        }

        qDebug() << "[Thread" << id << "] finished";
        activeThreads--;
    }
};


class MyMainWindow : public QMainWindow {
    int threadId = 1;

protected:
    void mousePressEvent(QMouseEvent * event) override {
        if (activeThreads >= MAX_THREADS) {
            qDebug() << "Max threadcount reached" << MAX_THREADS;
            return;
        }
        if (event->button() == Qt::RightButton) {
            qDebug() << "Right mouse click";
            activeThreads++;
        }
        else if (event->button() == Qt::LeftButton){
            qDebug() << "Left mouse click";
            activeThreads++;
        }
    }
};

int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    MyMainWindow w;
    w.show();
    return app.exec();
}