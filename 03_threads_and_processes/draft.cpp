#include <QThread>
#include <QDebug>
#include <QApplication>
#include <QMainWindow>
#include <QMouseEvent>
#include <QLabel>
#include <QVBoxLayout>

class Worker : public QObject {
    Q_OBJECT
    int id;
    bool ascending;
public:
    Worker(int id, bool asc) : id(id), ascending(asc) {}

public slots:
    void doWork() {
        emit message(QString("[worker %1] started").arg(id));
        if (ascending) {
            for (int i = 1; i <= 10; i++) {
                emit message(QString("[worker %1] %2").arg(id).arg(i));
                QThread::sleep(1);
            }
        } else {
            for (int i = 10; i >= 1; i--) {
                emit message(QString("[worker %1] %2").arg(id).arg(i));
                QThread::sleep(1);
            }
        }
        emit message(QString("[worker %1] finished").arg(id));
        emit finished();
    }

signals:
    void finished();
    void message(const QString &text);
};

class MyWindow : public QMainWindow {
    Q_OBJECT
    int nextId = 1;
    QLabel *label;
public:
    MyWindow() {
        auto *central = new QWidget;
        auto *layout = new QVBoxLayout;
        label = new QLabel("click left/right mouse to start a thread");
        layout->addWidget(label);
        central->setLayout(layout);
        setCentralWidget(central);
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        bool asc = event->button() == Qt::RightButton;

        auto *worker = new Worker(nextId++, asc);
        auto *thread = new QThread;

        worker->moveToThread(thread);

        connect(thread, &QThread::started, worker, &Worker::doWork);
        connect(worker, &Worker::finished, thread, &QThread::quit);
        connect(worker, &Worker::finished, worker, &QObject::deleteLater);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        // connect worker messages to a slot in this window
        connect(worker, &Worker::message, this, &MyWindow::showMessage);

        thread->start();
    }

private slots:
    void showMessage(const QString &text) {
        // this runs in the gui thread, so it's safe to update ui
        label->setText(text);
        qDebug() << text;
    }
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MyWindow w;
    w.show();
    return app.exec();
}
