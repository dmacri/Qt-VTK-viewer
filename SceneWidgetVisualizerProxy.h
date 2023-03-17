#ifndef SCENEWIDGETPROXY_H
#define SCENEWIDGETPROXY_H
#include "Parameter.h"
#include "Visualizer.h"

class SceneWidgetVisualizerProxy {
public:
    Parameter ** p;
    Visualizer<Parameter> *vis;
    SceneWidgetVisualizerProxy(){
        vis=new Visualizer<Parameter> ;
    }
    Parameter **  getAllocatedParametersMatrix(int dimX,int dimY){
        p = new Parameter *[dimY];
        for (int i = 0; i < dimY; i++)
        {
            p[i] = new Parameter[dimX];
        }
        return p;
    }
};
#endif
