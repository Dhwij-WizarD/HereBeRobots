#pragma once
#include <geometry.hpp>


namespace HBR::Geometry
{
    class Curves : public Geometry
    {
        public:
            Curves() = default;
            Curves(const Curves& other) = default;

            virtual SampledGeometry sample(double resolution) noexcept = 0;
            virtual void move(const Vec2d& delta) noexcept = 0;
            virtual void rotate(double angle, const Point& center) noexcept = 0;
            virtual void scale(double factor, const Point& center) noexcept = 0;

            virtual ~Curves() = default;
   };
}