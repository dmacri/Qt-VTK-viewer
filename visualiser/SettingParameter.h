#pragma once

#include <iosfwd>
#include <string>
#include "types.h"

struct SettingParameter
{
    StepIndex step;
    StepIndex nsteps;
    int numberOfColumnX;
    int numberOfRowsY;
    int nNodeX;
    int nNodeY;
    int numberOfLines;
    std::string outputFileName;
    static constexpr int font_size = 18;
    bool changed;
    bool firstTime;

    friend std::ostream& operator<<(std::ostream& os, const SettingParameter& sp);
};
