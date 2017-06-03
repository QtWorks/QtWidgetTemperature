#include "graduswidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GradusWidget w;
    w.show();

    return a.exec();
}
