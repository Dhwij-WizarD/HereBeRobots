#pragma once
#include <point.hpp>
#include <vec2d.hpp>
#include <vector>

namespace HBR::Geometry
{
    using SampledGeometry = std::vector<Point>;
    class Geometry
    {
        public:
            virtual SampledGeometry sample(double resolution) noexcept = 0;
            virtual void move(const Vec2d& delta) noexcept = 0;
            virtual void rotate(double angle, const Point& center) noexcept = 0;
            virtual void scale(double factor, const Point& center) noexcept = 0;


            virtual ~Geometry() = default;
    };
}// namespace HBR::Geometry
