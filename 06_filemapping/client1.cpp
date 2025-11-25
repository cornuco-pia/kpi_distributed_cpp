#include "common.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

namespace bip = boost::interprocess;

class Client1Window : public QMainWindow {
    Q_OBJECT
public:
    Client1Window() {
        setWindowTitle("Client 1");
        resize(300, 200);

        QWidget* central = new QWidget;
        QVBoxLayout* layout = new QVBoxLayout(central);

        layout->addWidget(new QLabel("Client 1 Data Payload:"));
        layout->addWidget(new QLabel("- RAM Size\n- External Disk\n- Screen Width"));

        QPushButton* btnSend = new QPushButton("Send Data to Server");
        layout->addWidget(btnSend);

        connect(btnSend, &QPushButton::clicked, this, &Client1Window::sendData);

        setCentralWidget(central);
    }

public slots:
    void sendData() {
        try {
            bip::managed_shared_memory segment(bip::open_only, ipc_constants::SHARED_MEM_NAME);
            bip::named_mutex mutex(bip::open_only, ipc_constants::MUTEX_NAME);

            auto res = segment.find<SharedData>("SharedData");
            if (!res.first) throw std::runtime_error("Server not running (SharedData not found)");
            SharedData* shm = res.first;

            {
                bip::scoped_lock<bip::named_mutex> lock(mutex);
                
                if (shm->new_data_flag != 0) {
                    QMessageBox::warning(this, "Busy", "Server hasn't read the previous message yet.");
                    return;
                }

                Client1Payload p;
                p.total_ram_kb = 16777216; // 16GB
                p.has_external_disk = true;
                p.screen_width_px = 1920;

                shm->client1_payload = p;
                shm->new_data_flag = 1;
            }

            QMessageBox::information(this, "Success", "Data sent to shared memory!");

        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Failed: %1").arg(e.what()));
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Client1Window w;
    w.show();
    return app.exec();
}

#include "client1.moc"