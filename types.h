/** @file types.h
 * @brief Common type definitions used throughout the application. */

#pragma once

using StepIndex = int;
using NodeIndex = int;
using FilePosition = long long;

struct ColumnAndRow
{
    using CoordinateType = int;

    CoordinateType column;
    CoordinateType row;

    static ColumnAndRow xy(CoordinateType x, CoordinateType y)
    {
        return ColumnAndRow {.column=x, .row=y};
    }

    auto x() const
    {
        return column;
    }
    auto y() const
    {
        return row;
    }
};
