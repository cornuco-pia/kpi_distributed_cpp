#include "../mainWindow.h"
#include "../runApp.cpp"
#include <sstream>

class Client2Window : public MainWindow {
    Q_OBJECT
public:
    explicit Client2Window(QWidget* parent = nullptr) : MainWindow(parent) {}

protected:
    std::string prepareData() override {
        int taskbarWidth = 1920 - 1850;  // placeholder
        int monitorCount = 2;
        int dpiX = 96;

        std::ostringstream ss;
        ss << "Client2 | Taskbar width: " << taskbarWidth
           << ", Monitors: " << monitorCount
           << ", DPI X: " << dpiX;
        return ss.str();
    }
};


int main(int argc, char* argv[]) {
    return runApp<Client2Window>(argc, argv, "Client 2");
}

//idc
#include "main.moc"