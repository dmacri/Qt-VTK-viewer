#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>

#include <QVTKOpenGLNativeWidget.h>
#include <QWidget>
#include <QtGui>

int main(int argc, char* argv[])
{
   // vtkObject::GlobalWarningDisplayOff();



    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
    QApplication a(argc, argv);
    QApplication::setStyle("fusion");
    // Imposta uno stile personalizzato per le finestre principali
    QString styleSheet = "QWidget {"
                         "    background-color: #f2f2f2;"
                         "    color: #333333;"
                         "    font-family: Arial, sans-serif;"
                         "}"
                         "QMenuBar {"
                         "    background-color: #444444;"
                         "    color: #ffffff;"
                         "}"
                         "QMenuBar::item {"
                         "    background-color: #444444;"
                         "    padding: 4px 12px;"
                         "}"
                         "QMenuBar::item:selected {"
                         "    background-color: #666666;"
                         "}";




    QWidget* parent = nullptr;

    MainWindow *mainWindow=new MainWindow(parent, argc,  argv);
    mainWindow->setStyleSheet(styleSheet);
    mainWindow->show();
    return a.exec();
}




