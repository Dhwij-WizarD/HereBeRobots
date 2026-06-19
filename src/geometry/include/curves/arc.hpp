#pragma once
#include <point.hpp>
#include <curves/line.hpp>
#include <vector>
#include <geometry.hpp>
#include <math.h>


namespace HBR::Geometry
{
class Arc final : public Geometry
{
public:
        ///@brief Constructor for Arc class, initializes the center coordinates and radius of the arc.
        ///@param x The x-coordinate of the center of the arc.
        ///@param y The y-coordinate of the center of the arc.
        ///@param radius The radius of the arc.
        ///@param start_angle The starting angle of the arc in RADIANS.
        ///@param end_angle The ending angle of the arc in RADIANS.
  Arc(double x, double y, double radius, double start_angle, double end_angle);

        ///@brief Calculates the length of the arc based on its radius and the angle subtended by the arc.
        ///@return The length of the arc.
  double get_length();


  virtual SampledGeometry sample(double resolution) noexcept override;
  virtual void move(const Vec2d & delta) noexcept override;
  virtual void rotate(double angle, const Point & center) noexcept override;
  virtual void scale(double factor, const Point & center) noexcept override;

  virtual ~Arc() = default;

private:
  double x_, y_, radius_, start_angle_, end_angle_;
  std::vector<Point> arc_points;

};
}
