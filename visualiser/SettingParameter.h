#pragma once

#include <iosfwd>
#include <string>
#include "visualiserProxy/SceneWidgetVisualizerProxyDefault.h"

class SceneWidget;


struct SettingParameter
{
    int step;
    int nsteps;
    bool changed;
    bool firstTime;
    bool insertAction; // TOO: GB: Not read anywhere, just set
    int dimX;
    int dimY;
    int nNodeX;
    int nNodeY;
    std::string outputFileName;
    int numberOfLines;
    const int font_size = 18;
    std::string edittext;// an empty string for editting
    SceneWidgetVisualizerProxy* sceneWidgetVisualizerProxy;
    SceneWidget* sceneWidget;

    friend std::ostream& operator<<(std::ostream& os, const SettingParameter& sp);
};
