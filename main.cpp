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


    QWidget* parent = nullptr;

    MainWindow *w=new MainWindow(parent, argc,  argv);

    w->show();
    return a.exec();
}




