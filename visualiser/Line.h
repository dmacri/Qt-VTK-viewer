/** @file Line.h
 * @brief Declaration of the Line structure for 2D line representation. */

#pragma once

/** @struct Line
 * @brief Represents a 2D line segment with start and end points.
 *
 * This structure is used to define 2D line segments in the visualization. */
struct Line
{
    float x1{}; ///< X-coordinate of the start point
    float y1{}; ///< Y-coordinate of the start point
    float x2{}; ///< X-coordinate of the end point
    float y2{}; ///< Y-coordinate of the end point

    /// @brief Default constructor (creates a zero-length line at origin)
    Line() = default;

    /** @brief Constructs a line with specified start and end points.
     *  @param x1 X-coordinate of the start point
     *  @param y1 Y-coordinate of the start point
     *  @param x2 X-coordinate of the end point
     *  @param y2 Y-coordinate of the end point */
    Line(float x1, float y1, float x2, float y2)
        : x1(x1), y1(y1), x2(x2), y2(y2)
    {}
};
