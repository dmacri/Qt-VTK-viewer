#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>
#include <filesystem>
#include <vtkGenericOpenGLRenderWindow.h>


int main(int argc, char* argv[])
{
   // vtkObject::GlobalWarningDisplayOff();

    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setApplicationName("Visualiser");

    MainWindow mainWindow;
    
    if (argc > 1)
    {
        const auto configurationFile = argv[1];
        if (std::filesystem::exists(configurationFile))
        {
            mainWindow.loadInitialConfiguration(configurationFile);
        }
        else
        {
            std::cerr << "Provided argument is not valid file path: '" << configurationFile << "'!" << std::endl;
        }
    }

    if (QFile styleFile("style.qss"); styleFile.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(styleFile.readAll());
        mainWindow.setStyleSheet(styleSheet);
    }

    mainWindow.show();
    return a.exec();
}
