#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <memory>

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QString>
#include <QDateTime>
#include <QTimer>

using boost::asio::ip::tcp;


const int SERVER_PORT = 5000;

class Session;

class ServerWindow : public QMainWindow {
    Q_OBJECT
    QTextEdit * log;
    tcp::acceptor tcp_acceptor;
public:
    ServerWindow(boost::asio::io_context& io_context)
        : tcp_acceptor(io_context, tcp::endpoint(tcp::v4(), SERVER_PORT)) 
        {
        setWindowTitle("Server");
        //setGeometry(100, 100, 450, 300); doesn't work for a tiling wm

        QWidget * central_widget = new QWidget;
        QVBoxLayout * layout = new QVBoxLayout(central_widget);

        log = new QTextEdit(this);
        log->setReadOnly(true);

        layout->addWidget(log);
        setCentralWidget(central_widget);

        logMessage("Server initiated");
        QTimer::singleShot(1000, this, &ServerWindow::do_accept);
    }

public slots:
    void logMessage(const QString& msg){
        std::cout << msg.toStdString() << std::endl;
        log->append(QDateTime::currentDateTime().toString("HH:mm:ss") + " > " + msg);
    }
private:
    void do_accept();
};


class Session : public QObject, public std::enable_shared_from_this<Session> {
    Q_OBJECT
    tcp::socket tcp_socket;
    ServerWindow* server_window;
    static inline int next_client_id = 0;
    int client_id;
    int data_int;
    boost::asio::streambuf buffer;
public:
    Session(tcp::socket socket, ServerWindow* window) : tcp_socket(std::move(socket)), server_window(window) {
        client_id = next_client_id++;
        connect(this, &Session::window_log_message, window, &ServerWindow::logMessage);

        QString msg = "Client " + QString::number(client_id) + " connected.";
        emit window_log_message(msg);
    }
    void start() {
        do_read();
    }
private:
    void do_read() {
        QString msg = "Client " + QString::number(client_id) + " reading.";
        emit window_log_message(msg);
        auto self(shared_from_this());
        boost::asio::async_read_until(tcp_socket, buffer, '\n', //delimiter
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                //data received
                std::istream is(&buffer); //make a stream from buffer
                std::string line;
                std::getline(is, line); //extract everything before \n and clears it from buffer
                emit window_log_message(QString("Client %1: %2").arg(client_id).arg(QString::fromStdString(line)));
                do_read();
            } else if (ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset){
                emit window_log_message(QString("Client %1 disconnected.").arg(client_id));
            }
            else {
                //std::cout << "Client " << client_id << " error: " << ec.message() << std::endl;
                emit window_log_message(QString("Client %1 error: %2").arg(client_id).arg(ec.message()));
            }
        });
    }
signals:
    void window_log_message(const QString &msg);

};

//after session bc it creates a session instance
void ServerWindow::do_accept() {
        //logMessage(QString("Server ready for connection")); 
        tcp_acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket){
            if (!ec) {
                std::make_shared<Session>(std::move(socket), this)->start();
                //logMessage(QString("Accepted a connection")); MAKES AN ERROR
            } else {
                //logMessage(QString("Acceptor generated an error"));
            }
            do_accept();

        });
    }

int main(int argc, char *argv[]) {
    std::cout.setf(std::ios::unitbuf); // to counter qt's execution loop
    std::cout << "server starting\n";
    QApplication app(argc, argv);

    boost::asio::io_context io_context;
    auto work_guard = boost::asio::make_work_guard(io_context);
    std::thread asio_thread([&io_context]() {io_context.run();});

    ServerWindow server_window(io_context);
    server_window.show();
    int result = app.exec();
    
    std::cout << "server stopped, cleaning...\n";
    work_guard.reset();
    io_context.stop();
    if (asio_thread.joinable()) { asio_thread.join(); }

    return result;
}


#include "server.moc"