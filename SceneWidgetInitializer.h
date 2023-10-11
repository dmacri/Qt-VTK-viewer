// scenewidgetinitializer.h
#ifndef SCENEWIDGETINITIALIZER_H
#define SCENEWIDGETINITIALIZER_H

#include "ui_mainwindow.h"
#include <QString>

class SceneWidgetInitializer
{
public:
    static void initializeSceneWidget(Ui::MainWindow* ui, int argc, char* argv[]);
};

#endif // SCENEWIDGETINITIALIZER_H
