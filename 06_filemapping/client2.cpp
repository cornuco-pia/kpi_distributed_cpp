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

class Client2Window : public QMainWindow {
    Q_OBJECT
public:
    Client2Window() {
        setWindowTitle("Client 2");
        resize(300, 200);

        QWidget* central = new QWidget;
        QVBoxLayout* layout = new QVBoxLayout(central);

        layout->addWidget(new QLabel("Client 2 Data Payload:"));
        layout->addWidget(new QLabel("- Status Bar Height\n- Notif Panel Width\n- DPI"));

        QPushButton* btnSend = new QPushButton("Send Data to Server");
        layout->addWidget(btnSend);

        connect(btnSend, &QPushButton::clicked, this, &Client2Window::sendData);

        setCentralWidget(central);
    }

public slots:
    void sendData() {
        try {
            bip::managed_shared_memory segment(bip::open_only, ipc_constants::SHARED_MEM_NAME);
            bip::named_mutex mutex(bip::open_only, ipc_constants::MUTEX_NAME);

            auto res = segment.find<SharedData>("SharedData");
            if (!res.first) throw std::runtime_error("Server not running");
            SharedData* shm = res.first;

            {
                bip::scoped_lock<bip::named_mutex> lock(mutex);
                
                if (shm->new_data_flag != 0) {
                    QMessageBox::warning(this, "Busy", "Server is busy processing other data.");
                    return;
                }

                Client2Payload p;
                p.status_bar_height_px = 24;
                p.notification_panel_width_px = 350;
                p.horizontal_dpi = 96;

                shm->client2_payload = p;
                shm->new_data_flag = 2; // Signal for Client 2
            }

            QMessageBox::information(this, "Success", "Data sent to shared memory!");

        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Failed: %1").arg(e.what()));
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Client2Window w;
    w.show();
    return app.exec();
}

#include "client2.moc"