#include <iostream>
#include "SettingParameter.h"

std::ostream& operator<<(std::ostream& os, const SettingParameter& sp)
{
    os << "SettingParameter{"
       << "dimX=" << sp.dimX << ", "
       << "dimY=" << sp.dimY << ", "
       << "nNodeX=" << sp.nNodeX << ", "
       << "nNodeY=" << sp.nNodeY << ", "
       << "outputFileName=" << sp.outputFileName << "}";
    return os;
}
