#pragma once

#include <point.hpp>
#include <geometry.hpp>
#include <vector>

namespace HBR::Geometry
{
class Polygon final : public Geometry
{
public:
        ///@brief Constructor for Polygon class, initializes the vertices of the polygon.
        ///@param vertices A vector of Point objects representing the vertices of the polygon.
  Polygon(const std::vector<Point> & vertices);

        ///@brief Calculates the perimeter of the polygon by summing the distances between consecutive vertices.
        ///@return The perimeter of the polygon.
  double get_perimeter();

        ///@brief Calculates the area of the polygon using the shoelace formula.
        ///@return The area of the polygon.
  double get_area();

  virtual SampledGeometry sample(double resolution) noexcept override;
  virtual void move(const Vec2d & delta) noexcept override;
  virtual void rotate(double angle, const Point & center) noexcept override;
  virtual void scale(double factor, const Point & center) noexcept override;

private:
  std::vector<Point> vertices_;
};
}
