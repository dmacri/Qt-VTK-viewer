#include <iostream>
#include "SettingParameter.h"

std::ostream& operator<<(std::ostream& os, const SettingParameter& sp)
{
    os << "SettingParameter{"
       << "numberOfColumnX=" << sp.numberOfColumnX << ", "
       << "numberOfRowsY=" << sp.numberOfRowsY << ", "
       << "nNodeX=" << sp.nNodeX << ", "
       << "nNodeY=" << sp.nNodeY << ", "
       << "outputFileName=" << sp.outputFileName << "}";
    return os;
}
