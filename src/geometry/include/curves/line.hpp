#pragma once
#include <point.hpp>
#include <geometry.hpp>

namespace HBR::Geometry
 {
    class Line final : public Geometry
    {
    public:

        ///@brief Constructor for Line class, initializes the start and end points of the line.
        ///@param start A Point object representing the starting point of the line.
        ///@param end A Point object representing the ending point of the line.
        Line(const Point& start, const Point& end);

        virtual SampledGeometry sample(double resolution) noexcept override;
        virtual void move(const Vec2d& delta) noexcept override;
        virtual void rotate(double angle, const Point& center) noexcept override;
        virtual void scale(double factor, const Point& center) noexcept override;
        
    private:
        Point start_;
        Point end_;

    };
}