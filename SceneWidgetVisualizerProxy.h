#ifndef SCENEWIDGETPROXY_H
#define SCENEWIDGETPROXY_H
#include "Visualizer.h"
#include "/home/davide/progetti-vtk/OOpenCAL/models/Ball/ballCell.h"
using namespace std;
class SceneWidgetVisualizerProxy {
public:
BallCell** p;
    Visualizer<BallCell> *vis;
    SceneWidgetVisualizerProxy() {
        vis = new Visualizer<BallCell>;
    }
    BallCell** getAllocatedParametersMatrix(int dimX, int dimY)
    {
        BallCell** p = new BallCell*[dimY];
        for (int i = 0; i < dimY; i++) {
            p[i] = new BallCell[dimX];
        }
        return p;
    }
};
#endif
