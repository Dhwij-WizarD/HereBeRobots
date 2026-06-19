#pragma once

#include "point.hpp"
#include <cmath>


namespace HBR::Geometry
{
class Vec2d
{
private:
  Point v;

public:
  Vec2d(double x, double y);
  Vec2d(const Point & p);
  double x() const;
  double y() const;
  Vec2d operator+(const Vec2d & other) const;
  Vec2d operator-(const Vec2d & other) const;
  Vec2d operator*(double scalar) const;
  Vec2d operator/(double scalar) const;

  double dot(const Vec2d & other) const;
  double cross(const Vec2d & other) const;

  double norm() const;

};
}
