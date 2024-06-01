#include <QtCore/qglobal.h>

#if QT_VERSION_MAJOR >= 6
#include <QtWidgets/QApplication>
#elif QT_VERSION_MAJOR >= 5
#include <QtWidgets/QApplication>
#else
#error Unsupported Qt version
#endif

#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
