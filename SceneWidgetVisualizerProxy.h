#ifndef SCENEWIDGETPROXY_H
#define SCENEWIDGETPROXY_H
#include "Visualizer.h"
#include "/home/dmacri80/Progetto-Visualizer/OOpenCAL/models/SciddicaT/SciddicaTCell.h"
using namespace std;
class SceneWidgetVisualizerProxy {
public:
SciddicaTCell** p;
    Visualizer<SciddicaTCell> *vis;
    SceneWidgetVisualizerProxy() {
        vis = new Visualizer<SciddicaTCell>;
    }
    SciddicaTCell** getAllocatedParametersMatrix(int dimX, int dimY)
    {
        SciddicaTCell** p = new SciddicaTCell*[dimY];
        for (int i = 0; i < dimY; i++) {
            p[i] = new SciddicaTCell[dimX];
        }
        return p;
    }
};
#endif
