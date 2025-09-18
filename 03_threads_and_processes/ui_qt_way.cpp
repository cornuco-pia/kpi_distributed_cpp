#include <QMainWindow>
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QVBoxLayout>
#include <QCursor>
#include <QTimer>
#include <QMenu>
#include <QMutex>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

int unused_variable = 42;

QLabel *mutexCounterLabel = nullptr;
QLabel *atomCounterLabel = nullptr;
int mutexSharedCounter = 0;
std::atomic<int> atomSharedCounter{0};
QMutex gCounterMutex;

class ClickableLabel : public QLabel { 
    Q_OBJECT
    int workerId;

public:
    ClickableLabel(int id, const QString &text, QWidget* parent = Q_NULLPTR) : QLabel(text, parent), workerId(id) {}

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            emit clicked();
            qDebug() << "emitted clicked";
        }
    }
};

class Worker : public QObject {
    Q_OBJECT
    bool ascending;
    bool running = true;
    int currentValue;
    QTimer *timer;
public:
    Worker(bool asc) : ascending(asc) {
        timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, this, &Worker::processNext);
        
        if (ascending) {
            currentValue = 1;
        } else {
            currentValue = 10;
        }

    }
public slots:
    void doWork() {
        qDebug() << "a thread has been started with pid:" << syscall(SYS_gettid);
        processNext();
    }
    void processNext() {
        emit progressUpdate(currentValue, ascending);
        //qDebug() << QString("running : %1").arg(running);
        if ((ascending && currentValue >= 10) || (!ascending && currentValue <= 1) || (!running)) {
            qDebug() << "worker checked finished";
            timer->stop();
            emit finished();
            return;
        }

        if (ascending) {
            currentValue++;
        } else {
            currentValue--;
        }

        timer->start(1000);
    }
    void stopRunning() {
        running = false;
        qDebug() << "stopped running";
    }
    signals:
        void progressUpdate(int number, bool ascending);
        void finished();
};

class WorkerController : public QObject {
    Q_OBJECT
    int myId;
    Worker * myWorker;
    ClickableLabel *myLabel;
    QThread *myThread;
    
public:
    WorkerController(QWidget * parent, int id, bool ascending, QPoint pos) : QObject(parent){
        myId = id;
        myWorker = new Worker(ascending);
        myThread = new QThread(); //CRUCIAL to keep track of the threads
        myLabel = new ClickableLabel(myId, QString("worker %1").arg(myId), parent);

        myLabel->move(parent->mapFromGlobal(pos));
        myLabel->resize(100, 50);
        myLabel->show();

        myWorker->moveToThread(myThread);

        connect(myWorker, &Worker::finished, this, &WorkerController::workerFinished);

        connect(myWorker, &Worker::progressUpdate, this, &WorkerController::handleWorkerUpdate);

        connect(myLabel, &ClickableLabel::clicked, myWorker, &Worker::stopRunning);

        connect(myThread, &QThread::started, myWorker, &Worker::doWork);
        myThread->start();

    }

protected:
    void printMessage(int number, bool ascending) {
        QString text = QString("[worker %1] %2 %3").arg(myId).arg(number).arg(ascending ? "^" : "v");
        //mainLabel->setText(text);
        qDebug() << text;
    }

    void updateLabel(int number, bool ascending) {
        QString text = QString("[worker %1] %2 %3").arg(myId).arg(number).arg(ascending ? "^" : "v");
            myLabel->setText(text);
    }   
signals: 
    void controllerFinished(int myId);

public slots:
    void workerFinished(){
        myLabel->deleteLater();
        myWorker->deleteLater();
        myThread->quit();
        myThread->wait();
        myThread->deleteLater();
        
        emit controllerFinished(myId);
    }
    void handleWorkerUpdate(int number, bool ascending){
        //printMessage(number, ascending);
        updateLabel(number, ascending);
        //upd global counter of ticks
        QMutexLocker locker(&gCounterMutex);
        mutexSharedCounter++;
        mutexCounterLabel->setText(QString("Mutex counter: %1").arg(mutexSharedCounter));

        atomSharedCounter++;
        atomCounterLabel->setText(QString("Atomic counter: %1").arg(atomSharedCounter.load()));
    }
    void finishWorker(){
        myWorker->stopRunning();
    }
};

class MyWindow : public QMainWindow{
    Q_OBJECT
    int nextId = 1;
    QLabel *mainLabel;
    QHash<int, WorkerController*> workerControllers;
    int maxThreads = 5;
    QMenu *contextMenu;
    
public:
    MyWindow() {
        auto *central = new QWidget;
        auto *layout = new QVBoxLayout;
        mainLabel = new QLabel("click left/right mouse to start a thread");
        layout->addWidget(mainLabel);
        central->setLayout(layout);
        setCentralWidget(central);
        setGeometry(200, 200, 800, 500); 

        contextMenu = new QMenu(this);
        for (int i = 1; i <= 10; i++) {
            QAction *action = contextMenu->addAction(QString::number(i));
            connect(action, &QAction::triggered, this, [this, i]() { 
                maxThreads = i; 
                qDebug() << "max threads set to" << maxThreads;
            });
        }
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &MyWindow::customContextMenuRequested, this, [this](const QPoint &pos){
            contextMenu->exec(mapToGlobal(pos));
        });

        mutexCounterLabel = new QLabel("Mutex counter: 0", this);
        mutexCounterLabel->move(10, 10);
        mutexCounterLabel->setFixedSize(150, 10);
        mutexCounterLabel->show();

        atomCounterLabel = new QLabel("Atomic counter: 0", this);
        atomCounterLabel->move(10, 30);
        atomCounterLabel->setFixedSize(150, 10);
        atomCounterLabel->show();
    }
    ~MyWindow() {
    for (WorkerController *c : workerControllers) {
        delete c;
    }
    workerControllers.clear();
    }
protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (workerControllers.size() >= maxThreads) return;
        bool asc = event->button() == Qt::RightButton;
        auto cursor_pos = QCursor().pos();

        auto *controller = new WorkerController(this, nextId, asc, cursor_pos);
        connect(controller, &WorkerController::controllerFinished, this, &MyWindow::handleFinishedController);
        workerControllers.insert(nextId, controller);
        nextId++;
    }
    void closeEvent(QCloseEvent *event) override {
        // for debug
        qDebug() << "Found threads:" << workerControllers.size();
        qDebug() << "Waiting for threads to finish...";

        for (WorkerController *c : workerControllers){
            c->finishWorker();
        }
        qDebug() << "Closed correctly";
        QMainWindow::closeEvent(event);
    }

public slots:
    void handleFinishedController(int controllerId) {
        delete workerControllers[controllerId]; //because deleteLater DOESN'T WORK correctly here
        workerControllers.remove(controllerId);
    }
};



int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MyWindow w;
    w.show();
    return app.exec();
}

#include "ui_qt_way.moc"