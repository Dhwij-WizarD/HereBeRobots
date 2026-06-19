#include <shapes/circle.hpp>
#include <cmath>

namespace HBR::Geometry
{
Circle::Circle(double x, double y, double radius)
{
  x_ = x;
  y_ = y;
  radius_ = radius;
}

double Circle::get_perimeter()
{
  return 2 * M_PI * radius_;
}

double Circle::get_area()
{
  return M_PI * radius_ * radius_;
}

SampledGeometry  Circle::sample(double resolution) noexcept
{
  for (double rad = 0; rad <= 2 * M_PI; rad += resolution) {
    double x = x_ + radius_ * cos(rad);
    double y = y_ + radius_ * sin(rad);
    circle_points.push_back(Point(x, y));
  }
  return circle_points;
}

void Circle::move(const Vec2d & delta) noexcept
{
  x_ += delta.x();
  y_ += delta.y();
}

void Circle::rotate(double angle, const Point & center) noexcept
{
  double radians = angle * M_PI / 180.0;
  double cos_angle = cos(radians);
  double sin_angle = sin(radians);

  double translated_x = x_ - center.x;
  double translated_y = y_ - center.y;

  double rotated_x = translated_x * cos_angle - translated_y * sin_angle;
  double rotated_y = translated_x * sin_angle + translated_y * cos_angle;

  x_ = rotated_x + center.x;
  y_ = rotated_y + center.y;
}

void Circle::scale(double factor, const Point & center) noexcept
{
  double translated_x = x_ - center.x;
  double translated_y = y_ - center.y;

  double scaled_x = translated_x * factor;
  double scaled_y = translated_y * factor;

  x_ = scaled_x + center.x;
  y_ = scaled_y + center.y;
}


}
