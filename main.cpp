#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QtGui>

int main(int argc, char* argv[])
{
   // vtkObject::GlobalWarningDisplayOff();



    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication a(argc, argv);
    QApplication::setStyle("fusion");

    QString styleSheet = "QWidget {"
                         "    background-color: #f2f2f2;"
                         "    color: #333333;"
                         "    font-family: Arial, sans-serif;"
                         "}"
                         "QMenuBar {"
                         "    background-color: #444444;"
                         "    color: #ffffff;"
                         "}"
                         "QStatusBar {"
                         "    background-color: #444444;"
                         "    color: #ffffff;"
                         "}"
                         "QMenuBar::item {"
                         "    background-color: #444444;"
                         "   "
                         "}"
                         "QMenuBar::item:selected {"
                         "    background-color: #666666;"
                         "}";




    QWidget* parent = nullptr;

    MainWindow *mainWindow=new MainWindow(parent, argc,  argv);
   // mainWindow->setStyleSheet(styleSheet);
    mainWindow->show();
    return a.exec();
}




