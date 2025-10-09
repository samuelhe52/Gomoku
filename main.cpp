#include <QApplication>

#include "UI/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setStyle("Fusion");
    MainWindow window;
    window.show();
    return QApplication::exec();
}
