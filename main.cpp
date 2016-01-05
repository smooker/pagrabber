#include "pagrabber.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PAGrabber w;
    w.show();

    return a.exec();
}
