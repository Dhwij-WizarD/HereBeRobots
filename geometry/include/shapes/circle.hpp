#pragma once
#include <shapes.hpp>
#include <point.hpp>
#include <vector>
#include <math.h>
#include <geometry.hpp>


namespace HBR::Geometry
 {
    class Circle final : public Shapes
    {
    public:
        ///@brief Constructor for Circle class, initializes the center coordinates and radius of the circle.
        ///@param x The x-coordinate of the center of the circle.
        ///@param y The y-coordinate of the center of the circle.
        ///@param radius The radius of the circle.
        Circle(double x, double y, double radius);

        ///@brief Calculates the perimeter of the circle using the formula 2 * π * radius.
        ///@return The perimeter of the circle.
        double get_perimeter() ;

        ///@brief Calculates the area of the circle using the formula π * radius^2.
        ///@return The area of the circle.
        double get_area() ;

        virtual SampledGeometry sample(double resolution) noexcept override;
        virtual void move(const Vec2d& delta) noexcept override;
        virtual void rotate(double angle, const Point& center) noexcept override;
        virtual void scale(double factor, const Point& center) noexcept override;
        
    private:
        double x_,y_,radius_;
        std::vector<Point> circle_points{}, arc_points{};

    };
}