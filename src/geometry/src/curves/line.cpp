#include <curves/line.hpp>

namespace HBR::Geometry
{

Line::Line(const Point & start, const Point & end)
: start_(start), end_(end)
{
}

SampledGeometry  Line::sample(double resolution) noexcept
{
  SampledGeometry line;
  double delta_x = end_.x - start_.x;
  double delta_y = end_.y - start_.y;
  double slope = atan2(delta_y, delta_x);
  double length = sqrt(delta_x * delta_x + delta_y * delta_y);
  for (double i = 0; i <= length ; i += resolution) {
    double x = start_.x + i * cos(slope);
    double y = start_.y + i * sin(slope);
    line.push_back({x, y});
  }
  return line;
}

void Line::move(const Vec2d & delta) noexcept
{
  start_.x += delta.x();
  start_.y += delta.y();
  end_.x += delta.x();
  end_.y += delta.y();
}

void Line::rotate(double angle, const Point & center) noexcept
{
  double radians = angle * M_PI / 180.0;
  double cos_angle = cos(radians);
  double sin_angle = sin(radians);

  auto rotate_point = [&](Point & point)
    {
      double translated_x = point.x - center.x;
      double translated_y = point.y - center.y;

      double rotated_x = translated_x * cos_angle - translated_y * sin_angle;
      double rotated_y = translated_x * sin_angle + translated_y * cos_angle;

      point.x = rotated_x + center.x;
      point.y = rotated_y + center.y;
    };

  rotate_point(start_);
  rotate_point(end_);
}

void Line::scale(double factor, const Point & center) noexcept
{
  auto scale_point = [&](Point & point)
    {
      double translated_x = point.x - center.x;
      double translated_y = point.y - center.y;

      double scaled_x = translated_x * factor;
      double scaled_y = translated_y * factor;

      point.x = scaled_x + center.x;
      point.y = scaled_y + center.y;
    };

  scale_point(start_);
  scale_point(end_);
}


}
