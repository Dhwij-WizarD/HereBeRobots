#include <vec2d.hpp>

namespace HBR::Geometry
{
Vec2d::Vec2d(double x, double y)
: v(x, y)
{
}

Vec2d::Vec2d(const Point & p)
: v(p.x, p.y)
{
}

double Vec2d::x() const
{
  return v.x;
}

double Vec2d::y() const
{
  return v.y;
}

Vec2d Vec2d::operator+(const Vec2d & other) const
{
  return Vec2d(v.x + other.v.x, v.y + other.v.y);
}

Vec2d Vec2d::operator-(const Vec2d & other) const
{
  return Vec2d(v.x - other.v.x, v.y - other.v.y);
}

Vec2d Vec2d::operator*(double scalar) const
{
  return Vec2d(v.x * scalar, v.y * scalar);
}

Vec2d Vec2d::operator/(double scalar) const
{
  return Vec2d(v.x / scalar, v.y / scalar);
}

double Vec2d::dot(const Vec2d & other) const
{
  return v.x * other.v.x + v.y * other.v.y;
}

double Vec2d::cross(const Vec2d & other) const
{
  return v.x * other.v.y - v.y * other.v.x;
}

double Vec2d::norm() const
{
  return sqrt(v.x * v.x + v.y * v.y);
}
}
