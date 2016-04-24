#include "samegamewindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SameGameWindow w;
    w.show();

    return a.exec();
}
