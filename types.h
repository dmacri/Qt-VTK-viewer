#pragma once

using StepIndex = int;
using NodeIndex = int;
using FilePosition = long long;

struct ColumnAndRow
{
    using CoordinateType = int;
    CoordinateType column;
    CoordinateType row;
};
