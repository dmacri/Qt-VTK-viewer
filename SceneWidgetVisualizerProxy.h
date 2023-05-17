#ifndef SCENEWIDGETPROXY_H
#define SCENEWIDGETPROXY_H
#include "Visualizer.h"
#include "/home/davide/progetti-vtk/OOpenCAL/Base/Models/Ball_New/Parameter.h"
using namespace std;
class SceneWidgetVisualizerProxy {
public:
Parameter** p;
    Visualizer<Parameter> *vis;
    SceneWidgetVisualizerProxy() {
        vis = new Visualizer<Parameter>;
    }
    Parameter** getAllocatedParametersMatrix(int dimX, int dimY)
    {
        Parameter** p = new Parameter*[dimY];
        for (int i = 0; i < dimY; i++) {
            p[i] = new Parameter[dimX];
        }
        return p;
    }
};
#endif
