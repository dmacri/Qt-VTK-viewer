#pragma once

struct Line
{
    float x1{}, y1{};
    float x2{}, y2{};

    Line() = default;

    Line(float x1, float y1, float x2, float y2)
        : x1(x1), y1(y1), x2(x2), y2(y2)
    {}
};
