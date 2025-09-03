// qt gui
#include <QApplication>
#include <QPalette>
#include <QCursor>

// qt widgets
#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>

// qt core
#include <QString>
#include <QTimer>

class MainWindow : public QMainWindow {
public:
    MainWindow() {
        setWindowTitle("name123");    // req
        setGeometry(50, 90, 400, 500);   // int x,int y,int w,int h     // req 
        //setWindowFlag(Qt::WindowCloseButtonHint, false); // req

        QPalette pal = palette();
        pal.setColor(QPalette::Window, QColor(64, 64, 64)); //req
        setAutoFillBackground(true);
        setPalette(pal);

        setCursor(Qt::SizeAllCursor); //req

        // horizontal scroll only
        QScrollArea *scrollArea = new QScrollArea(this);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setCentralWidget(scrollArea);

        // system metrics
        label = new QLabel(getMetricsText(), this);
        label->setStyleSheet("color: white;");
        scrollArea->setWidget(label);

        // timer connect TODO: check why this is passed separately
        QTimer *update_timer = new QTimer (this);
        connect(update_timer, &QTimer::timeout, this, &MainWindow::UpdateMetrics);
        update_timer->start(100);

        setWindowIcon(QIcon(":/Civic_machine_assimilator.png"));
        //setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint); //https://doc.qt.io/qt-6/qt.html#WindowType-enum
    }

private:
    QLabel *label;
    void UpdateMetrics(){
        QString new_label_text = getMetricsText();
        label->setText(new_label_text);
    }

    QString getMetricsText() {
        int cursorW = QCursor().pos().x();
        int cursorH = QCursor().pos().y();

        QSize client = size();

        QString text;
        text += "System metrics:\n";
        text += "Cursor width: " + QString::number(cursorW) + "\n";
        text += "Cursor height: " + QString::number(cursorH) + "\n";
        text += "Client area width: " + QString::number(client.width()) + "\n";
        text += "Client area height: " + QString::number(client.height()) + "\n";

        return text;
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}
