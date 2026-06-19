#pragma once

namespace HBR::Geometry
{
class Point
{
public:
        ///@brief Constructor for Point class, initializes the x and y coordinates of the point.
        ///@param x The x-coordinate of the point.
        ///@param y The y-coordinate of the point.
  Point(double x, double y);

  double x, y;

};
}
