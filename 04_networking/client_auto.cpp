#include <iostream>
#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include <string>

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QPushButton>

//for metrics retrieval
#include <chrono>
#include <sys/sysinfo.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

int get_system_uptime_seconds() {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return info.uptime; // uptime in seconds
    }
    return -1;
}

std::pair<int,int> get_screen_resolution() {
    Display* d = XOpenDisplay(nullptr);
    if (!d) return {-1,-1};
    Screen* s = DefaultScreenOfDisplay(d);
    int width = s->width;
    int height = s->height;
    XCloseDisplay(d);
    return {width, height};
}

using boost::asio::ip::tcp;

const int SERVER_PORT = 5000;
const std::string SERVER_HOST = "127.0.0.1";
const int RETRY_INTERVAL_MS = 3000;
const int SEND_INTERVAL_MS = 3000;

class ClientAuto;

class ClientWindow : public QMainWindow {
    Q_OBJECT
    std::shared_ptr<ClientAuto> client;
    QTimer * connected_timer;
    QTextEdit * log;
public:
    ClientWindow(boost::asio::io_context& io_context, int client_type) : client(std::make_shared<ClientAuto>(io_context, this, client_type)){
        setWindowTitle("Client auto");

        QWidget *centralWidget = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        log = new QTextEdit(this);
        log->setReadOnly(true);

        layout->addWidget(log);
        setCentralWidget(centralWidget);

        connected_timer = new QTimer(this);
        connect(connected_timer, &QTimer::timeout, this, &ClientWindow::try_connect_or_send);
        //QTimer::singleShot(200, this, &ClientWindow::timer_test);

        connected_timer->start(RETRY_INTERVAL_MS);
        logMessage(QString("Connection address is %1:%2...").arg(SERVER_HOST).arg(SERVER_PORT));
    }
    
    
private slots:
    void try_connect_or_send();

public slots:
    void on_connected() {
        logMessage("Connection established. Starting data transfer.");
        connected_timer->stop();
        connected_timer->start(SEND_INTERVAL_MS);
    }
    void logMessage(const QString& message) {
        std::cout << message.toStdString() << std::endl;
        log->append(QDateTime::currentDateTime().toString("HH:mm:ss") + " > " + message);
    }

    void on_disconnected() {
        connected_timer->stop();
        connected_timer->start(RETRY_INTERVAL_MS);
        logMessage("Switched to retry mode...");
    }
};

class ClientAuto : public QObject, public std::enable_shared_from_this<ClientAuto> {
    Q_OBJECT
    boost::asio::io_context& io_context;
    tcp::socket tcp_socket;
    ClientWindow* window;
    int counter;
    int client_type;
    tcp::resolver resolver;
    tcp::resolver::query query;
    tcp::resolver::results_type resolved_results;
public:
    ClientAuto(boost::asio::io_context & io_context, ClientWindow* window, int client_type)
    : QObject(nullptr), io_context(io_context), tcp_socket(io_context), window(window), counter(0), resolver(io_context), query(SERVER_HOST, std::to_string(SERVER_PORT)), client_type(client_type) {
        connect(this, &ClientAuto::server_connection_established, window, &ClientWindow::on_connected);
        connect(this, &ClientAuto::window_log_message, window, &ClientWindow::logMessage);
        connect(this, &ClientAuto::server_connection_closed, window, &ClientWindow::on_disconnected);
    }

    void start_connect() {
        if (tcp_socket.is_open()) {emit window_log_message("tcp socket already open"); return;}

        //strictly this is not needed as we pass local machine
        //but let it be
        resolver.async_resolve(query, [this, self = shared_from_this()] (
            const boost::system::error_code& ec, tcp::resolver::results_type results) {
                if (!ec) {
                    resolved_results = results;
                    emit window_log_message("resolved address");
                    boost::asio::async_connect(tcp_socket, resolved_results, [this, self]
                        (const boost::system::error_code &ec, const tcp::endpoint& endpoint) {
                            emit window_log_message("async_connect called");
                            if (!ec) {
                                emit server_connection_established();
                            } else {
                                emit window_log_message("Connection failed. Retrying...");
                            }

                        });
                } else {
                    QTimer::singleShot(0, [this, ec] {
                        emit window_log_message("DNS failed. Retrying...");
                    });
                }
            });
    }
    void send_next() {
        if (!tcp_socket.is_open()) return;
        auto self = shared_from_this();
        QString msg {QString("DEFAULT_MSG")};

        if (client_type == 0) {
            int system_uptime_seconds = get_system_uptime_seconds();
            msg = QString("system uptime is %1 s").arg(system_uptime_seconds);
            emit window_log_message(QString("Sending %1").arg(msg));
        
        } else if (client_type == 1) {
            std::pair<int,int> screen_resolution = get_screen_resolution();
            msg = QString("screen resolution is %1 by %2").arg(screen_resolution.first).arg(screen_resolution.second);
            emit window_log_message(QString("Sending %1").arg(msg));
        }

        msg.append('\n'); //delimiter bc reading arbitrary strings is not safe
        auto data_ptr = std::make_shared<QByteArray>(msg.toUtf8());

        boost::asio::async_write(tcp_socket, boost::asio::buffer(*data_ptr),
        [this, self, data_ptr](const boost::system::error_code &ec, std::size_t) {
            if (!ec) {
                emit window_log_message(QString("Sent."));
            } else {
                emit window_log_message("Send failed. Server connection lost.");
                tcp_socket.close();
                emit server_connection_closed();
            }
        });
    }
    bool is_connected() const {return tcp_socket.is_open();}
signals:
    void server_connection_established();
    void window_log_message(const QString &msg);
    void server_connection_closed();
};

void ClientWindow::try_connect_or_send() {
        if (!client->is_connected()){
            client->start_connect();
        } else {
            client->send_next();
        }
    }

int main(int argc, char *argv[]) {
    int client_type {0};
    if (argc > 1) {
        try {
            client_type = std::stoi(argv[1]);
            if (client_type > 1) {
                throw("client type index too large");
            } else if (client_type < 0){
                throw("client type index is less than 0");
            }
        }   
        catch (const char * errorMessage) {
            std::cout << "error processing argument, using default. error: " << errorMessage;
        } catch (const std::exception &e) {
        std::cout << "error processing argument, using default. error: " << e.what() << "\n";
        client_type = 0;
    }}
    std::cout << "client created\n";
    QApplication app(argc, argv);

    boost::asio::io_context io_context;
    auto work_guard = boost::asio::make_work_guard(io_context); // so that it doesn't exit prematurely
    std::thread asio_thread([&io_context]() {io_context.run();});

    ClientWindow w(io_context, client_type);
    w.show();

    int result = app.exec();

    work_guard.reset(); // allow io_context to finish
    io_context.stop();
    if (asio_thread.joinable()) {asio_thread.join();}
    return result;
}

#include "client_auto.moc"