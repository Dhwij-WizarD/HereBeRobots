#pragma once
#include <point.hpp>
#include <geometry.hpp>
#include <vec2d.hpp>


namespace HBR::Simulator
{
    /// @brief Will contain the unviersal properties
class Entity
{
private:
  enum class EntityType {ROBOT, OBSTACLE};

public:
  Entity() = default;
  virtual void move(const Geometry::Vec2d & delta) noexcept = 0;
  virtual void rotate(double angle, const Geometry::Point & center) noexcept = 0;
  virtual void scale(double factor, const Geometry::Point & center) noexcept = 0;
  virtual ~Entity() = default;

protected:

};
}// namespace HBR::Simulator
