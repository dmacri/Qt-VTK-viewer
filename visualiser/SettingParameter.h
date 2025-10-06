#pragma once

#include <iosfwd>
#include <string>
#include "types.h"

struct SettingParameter
{
    StepIndex step;
    StepIndex nsteps;
    int dimX;
    int dimY;
    int nNodeX;
    int nNodeY;
    int numberOfLines;
    std::string outputFileName;
    static constexpr int font_size = 18;
    bool changed;
    bool firstTime;
    bool insertAction; // TOO: GB: Not read anywhere, just set

    friend std::ostream& operator<<(std::ostream& os, const SettingParameter& sp);
};
