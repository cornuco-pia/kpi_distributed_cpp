#include "client3Window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <sstream>
#include "../runApp.cpp"

Client3Window::Client3Window(QWidget* parent)
    : MainWindow(parent)
{
    // Add extra inputs *before* sendButton in the layout
    auto* layout = qobject_cast<QVBoxLayout*>(centralWidget()->layout());
    if (!layout) return;

    auto* inputLayout = new QHBoxLayout;
    aInput = new QLineEdit(this);
    bInput = new QLineEdit(this);
    aInput->setPlaceholderText("Enter a");
    bInput->setPlaceholderText("Enter b");
    aInput->setValidator(new QIntValidator(0, 10000, this));
    bInput->setValidator(new QIntValidator(0, 10000, this));

    inputLayout->addWidget(new QLabel("a:"));
    inputLayout->addWidget(aInput);
    inputLayout->addWidget(new QLabel("b:"));
    inputLayout->addWidget(bInput);

    layout->insertLayout(0, inputLayout);  // insert above the send button
}

std::string Client3Window::prepareData() {
    bool okA, okB;
    int a = aInput->text().toInt(&okA);
    int b = bInput->text().toInt(&okB);

    if (!okA || !okB) {
        return "Invalid input";
    }

    int area = a * b;
    std::ostringstream ss;
    ss << "Client3 | Rectangle " << a << "x" << b << " area = " << area;
    return ss.str();
}

int main(int argc, char* argv[]) {
    return runApp<Client3Window>(argc, argv, "Client 3");
}

