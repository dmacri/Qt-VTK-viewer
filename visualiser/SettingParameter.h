#pragma once

#include <iosfwd>
#include <string>


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

    friend std::ostream& operator<<(std::ostream& os, const SettingParameter& sp);
};
