#include "../mainWindow.h"
#include <sstream>
#include "../runApp.cpp"

class Client1Window : public MainWindow {
    Q_OBJECT
public:
    explicit Client1Window(QWidget* parent = nullptr)
        : MainWindow(parent) {}

protected:
    std::string prepareData() override {
        // placeholders
        int captionHeight = 25; 
        bool keyboardPresent = true;
        int screenHeight = 1080;

        std::ostringstream ss;
        ss << "Client1 | caption height: " << captionHeight
           << ", keyboard: " << (keyboardPresent ? "yes" : "no")
           << ", screen height: " << screenHeight;
        return ss.str();
    }
};

int main(int argc, char* argv[]) {
    return runApp<Client1Window>(argc, argv, "Client 1");
}

//idc
#include "main.moc"