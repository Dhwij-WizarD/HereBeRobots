#include <curves/arc.hpp>

namespace HBR::Geometry
{
 Arc::Arc(double x, double y, double radius, double start_angle, double end_angle) 
{
    x_ = x;
    y_ = y;
    radius_ = radius;
    start_angle_ = start_angle;
    end_angle_ = end_angle;
}

double Arc::get_length() 
{
    return radius_ * (end_angle_ - start_angle_);
}

SampledGeometry  Arc::sample(double resolution) noexcept
{
    for (double radian = start_angle_; radian <= end_angle_; radian += resolution)
    {
        double x = x_ + radius_ * cos(radian);
        double y = y_ + radius_ * sin(radian);
        arc_points.push_back(Point(x, y));
    }
    return arc_points;
}   

void Arc::move(const Vec2d& delta) noexcept
{
    x_ += delta.x();
    y_ += delta.y();
}

void Arc::rotate(double angle, const Point& center) noexcept
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

void Arc::scale(double factor, const Point& center) noexcept
{
    double translated_x = x_ - center.x;
    double translated_y = y_ - center.y;

    double scaled_x = translated_x * factor;
    double scaled_y = translated_y * factor;

    x_ = scaled_x + center.x;
    y_ = scaled_y + center.y;

}


}