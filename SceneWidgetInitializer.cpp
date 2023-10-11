// scenewidgetinitializer.cpp
#include "SceneWidgetInitializer.h"

void SceneWidgetInitializer::initializeSceneWidget(Ui::MainWindow* ui, int argc, char* argv[])
{
    ui->sceneWidget->addVisualizer(argc, argv);
}
