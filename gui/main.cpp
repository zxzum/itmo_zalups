#include <QApplication>
#include "mainwindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("ITMO Loops");
    app.setOrganizationName("ITMO");
    app.setApplicationVersion("1.0");
    
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
