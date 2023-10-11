#include <cstdio>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    FILE *fptr;
    fptr = fopen("SceneWidgetVisualizerProxy.h", "w");
    fprintf(fptr, "#ifndef SCENEWIDGETPROXY_H\n");
    fprintf(fptr, "#define SCENEWIDGETPROXY_H\n");
    string s = "#include \"" + string(argv[1]) + "\"\n";
    fprintf(fptr, "#include \"Visualizer.h\"\n");
    fprintf(fptr, "%s", s.c_str());
    fprintf(fptr, "using namespace std;\n");
    fprintf(fptr, "class SceneWidgetVisualizerProxy {\n");
    fprintf(fptr, "public:\n");
    string parameter=string(argv[2])+"** p;\n";
    fprintf(fptr, "%s",parameter.c_str()); 
    string s1 = "    Visualizer<" + string(argv[2]) + "> *vis;\n";
    fprintf(fptr, "%s", s1.c_str());
    fprintf(fptr, "    SceneWidgetVisualizerProxy() {\n");
    string s2 = "        vis = new Visualizer<" + string(argv[2]) + ">;\n";
    fprintf(fptr, "%s", s2.c_str());
    fprintf(fptr, "    }\n");
    string sAllocateParameter = "    " + string(argv[2]) + "** getAllocatedParametersMatrix(int dimX, int dimY)\n";
    fprintf(fptr, "%s", sAllocateParameter.c_str());
    fprintf(fptr, "    {\n");
    string sInstantiateParameter = "        " + string(argv[2]) + "** p = new " + string(argv[2]) + "*[dimY];\n";
    fprintf(fptr, "%s", sInstantiateParameter.c_str());
    fprintf(fptr, "        for (int i = 0; i < dimY; i++) {\n");
    string sInstantiateParameterX = "            p[i] = new " + string(argv[2]) + "[dimX];\n";
    fprintf(fptr, "%s", sInstantiateParameterX.c_str());
    fprintf(fptr, "        }\n");
    fprintf(fptr, "        return p;\n");
    fprintf(fptr, "    }\n");
    fprintf(fptr, "};\n");
    fprintf(fptr, "#endif\n");
    fclose(fptr);
    return 0;
}
