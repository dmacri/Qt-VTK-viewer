#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>


int main(int argc, char* argv[])
{
    if (1 == argc)
    {
        std::cerr << "Usage: " << argv[0] << " <configurationFilePath>";
        return 1;
    }

   // vtkObject::GlobalWarningDisplayOff();

    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    MainWindow mainWindow(argc, argv);

    if (QFile styleFile("style.qss"); styleFile.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(styleFile.readAll());
        mainWindow.setStyleSheet(styleSheet);
    }

    mainWindow.show();
    return a.exec();
}
