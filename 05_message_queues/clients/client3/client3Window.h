#pragma once
#include "../mainWindow.h"
#include <QLineEdit>
#include <QLabel>

class Client3Window : public MainWindow {
    Q_OBJECT
public:
    Client3Window(QWidget* parent = nullptr);

protected:
    std::string prepareData() override;

private:
    QLineEdit* aInput;
    QLineEdit* bInput;
};
