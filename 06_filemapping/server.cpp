#include "common.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QDateTime>
#include <QString>
#include <QLabel>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>

namespace bip = boost::interprocess;

class ShmMonitor : public QObject {
    Q_OBJECT
public:
    std::atomic<bool> keep_running{true};

    void run() {
        emit logMessage("Monitor thread started. Waiting for shared memory...");
        
        bip::managed_shared_memory segment;

        std::unique_ptr<bip::named_mutex> mutex;
        // cannot define and not init this variable so switched to pointer
        //bip::named_mutex mutex;
        
        SharedData* shared_data = nullptr;

        try {
            // remove old resources
            std::cout << "REMOVING PREVIOUS OBJECTS" << std::flush;
            bip::shared_memory_object::remove(ipc_constants::SHARED_MEM_NAME);
            //bip::named_mutex::remove(ipc_constants::MUTEX_NAME);

            // create new
            //std::cout << "CREATING SHM" << std::flush;
            segment = bip::managed_shared_memory(bip::create_only, ipc_constants::SHARED_MEM_NAME, ipc_constants::SHARED_MEM_SIZE);
            //std::cout << "CREATING MUTEX" << std::flush;
            //mutex = std::make_unique<bip::named_mutex>(bip::create_only, ipc_constants::MUTEX_NAME);
            //std::cout << "DONE" << std::flush;
            //mutex = bip::named_mutex(bip::create_only, ipc_constants::MUTEX_NAME);
            
            shared_data = segment.construct<SharedData>("SharedData")();
            shared_data->new_data_flag = 0; // Init flag
            
            emit logMessage("Shared Memory created successfully.");

        } catch (const std::exception& e) {
            emit logMessage(QString("Error creating SHM: %1").arg(e.what()));
            return;
        }

        while (keep_running) {
            try {
                bip::scoped_lock<bip::interprocess_mutex> lock(shared_data->mutex);

                while (shared_data->new_data_flag == 0 && keep_running) {
                    shared_data->data_cond.wait(lock);
                }

                if (!keep_running) break;

                if (shared_data->new_data_flag == 1) {
                QString msg = QString("<b>[Client 1 Event]</b><br>RAM: %1<br>Width: %2")
                                .arg(shared_data->client1_payload.total_ram_kb)
                                .arg(shared_data->client1_payload.screen_width_px);
                emit logMessage(msg);
                shared_data->new_data_flag = 0;
                } 
                else if (shared_data->new_data_flag == 2) {
                    QString msg = QString("<b>[Client 2 Event]</b><br>Bar: %1<br>DPI: %2")
                                    .arg(shared_data->client2_payload.status_bar_height_px)
                                    .arg(shared_data->client2_payload.horizontal_dpi);
                    emit logMessage(msg);
                    shared_data->new_data_flag = 0;
                }
                } catch (const std::exception& e) {
                    emit logMessage(QString("Monitor Error: %1").arg(e.what()));
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
            }
        
        bip::shared_memory_object::remove(ipc_constants::SHARED_MEM_NAME);
        bip::named_mutex::remove(ipc_constants::MUTEX_NAME);
        emit logMessage("Shared memory cleaned up.");
    }

signals:
    void logMessage(const QString& msg);
};


class ServerWindow : public QMainWindow {
    Q_OBJECT
    QTextEdit* log;
    ShmMonitor* monitor;
    std::thread monitorThread;

public:
    ServerWindow() {
        setWindowTitle("Server (File Mapping)");
        resize(500, 400);

        QWidget* central = new QWidget;
        QVBoxLayout* layout = new QVBoxLayout(central);

        QLabel* lbl = new QLabel("Listening for incoming connections via Shared Memory...");
        layout->addWidget(lbl);

        log = new QTextEdit(this);
        log->setReadOnly(true);
        layout->addWidget(log);

        setCentralWidget(central);

        monitor = new ShmMonitor();
        connect(monitor, &ShmMonitor::logMessage, this, &ServerWindow::appendLog);

        monitorThread = std::thread(&ShmMonitor::run, monitor);
    }

    ~ServerWindow() {
        if (monitor) monitor->keep_running = false;
        if (monitorThread.joinable()) monitorThread.join();
        delete monitor;
    }

public slots:
    void appendLog(const QString& msg) {
        QString time = QDateTime::currentDateTime().toString("HH:mm:ss");
        log->append(time + " > " + msg);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ServerWindow w;
    w.show();
    return app.exec();
}

#include "server.moc"