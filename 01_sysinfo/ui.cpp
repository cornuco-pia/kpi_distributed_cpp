// qt gui
#include <QApplication>
#include <QPalette>
#include <QCursor>
#include <QFontMetricsF>

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
        setWindowTitle("trofymenko");    // req
        setGeometry(50, 90, 400, 500);   // int x,int y,int w,int h     // req 

        QPalette pal = palette();
        pal.setColor(QPalette::Window, Qt::lightGray); //req
        setAutoFillBackground(true);
        setPalette(pal);

        setCursor(Qt::WhatsThisCursor); //req

        // horizontal scroll only
        QScrollArea *scrollArea = new QScrollArea(this);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setCentralWidget(scrollArea);

        // system metrics
        label = new QLabel(getMetricsText(), this);
        label->setStyleSheet("color: white;");
        scrollArea->setWidget(label);

        QTimer *update_timer = new QTimer (this);
        connect(update_timer, &QTimer::timeout, this, &MainWindow::UpdateMetrics);
        update_timer->start(100);

        setWindowIcon(QIcon(":/Civic_machine_assimilator.png"));
        setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::WindowSystemMenuHint);
    }

private:
    QLabel *label;
    void UpdateMetrics(){
        QString new_label_text = getMetricsText();
        label->setText(new_label_text);
    }

    QString getMetricsText() {
        QFontMetricsF fm(this->font()); 

        int cursorW = QCursor().pos().x();
        int cursorH = QCursor().pos().y();

        QSize client = size();

        QString text;
        text += "System metrics:\n";
        text += "Cursor width: " + QString::number(cursorW) + "\n";
        text += "Cursor height: " + QString::number(cursorH) + "\n";
        text += "Client area width: " + QString::number(client.width()) + "\n";
        text += "Client area height: " + QString::number(client.height()) + "\n";
        text += "Font height: " + QString::number(fm.height()) + "\n";
        text += "Font linespacing: " + QString::number(fm.lineSpacing()) + "\n";

        return text;
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}
