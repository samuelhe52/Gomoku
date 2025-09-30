#include <QApplication>
#include <QPushButton>
#include <thread>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QPushButton button("Hello world!", nullptr);
    button.setText("My text");
    button.resize(300, 200);
    button.show();

    return QApplication::exec();
}
