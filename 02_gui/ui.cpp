#include <QMainWindow>
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QResizeEvent>
#include "myimage.h"


static const uchar bitmap_height = 8;
static const uchar bitmap_width = 8;
static const uchar bitmap_data[8 * 8 * 3] = {
    255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,
    0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,
    255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,
    0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,
    255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,
    0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,
    255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,
    0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,   0,0,0,   255,255,255,
};

class BitmapWidget : public QWidget {
    Q_OBJECT

public:
    BitmapWidget(QWidget *parent = nullptr, bool use_image = false) : QWidget(parent) {
        if (use_image == true){
        // TODO: check what exactly is happening with channels
        // fix rgba ?
        image = QImage(image_data, image_width, image_height, image_width * 3, QImage::Format_RGB888);
        }
        else {
        image = QImage(bitmap_data, bitmap_width, bitmap_height, QImage::Format_RGB888);
        }
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event); // also possible to manually cast (void)event or skip the var name in arguments
        QPainter painter(this);

        QRect targetRect(0, 0, width(), height());

        painter.drawImage(targetRect, image);
    }

private:
    QImage image;
};

int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    
    BitmapWidget checkerboard;
    BitmapWidget image(nullptr, true);

    checkerboard.show();
    image.show();

    return app.exec();
}

#include "ui.moc" // better to make a separate header file