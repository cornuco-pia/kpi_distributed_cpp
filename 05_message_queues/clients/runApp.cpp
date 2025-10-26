#include <QApplication>
#include <QString>
#include <memory>

template <typename T>
int runApp(int argc, char* argv[], const QString& windowName) {
    QApplication app(argc, argv);

    auto window = std::make_unique<T>();
    window->setWindowTitle(windowName);
    window->show();

    return app.exec();
}