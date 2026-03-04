#include <curves/line.hpp>
#include <geometry.hpp>
#include <cstddef>
#include <shapes/polygon.hpp>
namespace HBR::Geometry {

Polygon::Polygon(const std::vector<Point>& vertices) 
{
    vertices_ = vertices;
}

double Polygon::get_perimeter() 
{
    double perimeter = 0.0;
    for (std::size_t i = 0; i < vertices_.size(); ++i) 
    {
        const Point& current = vertices_[i];
        const Point& next = vertices_[(i + 1) % vertices_.size()];
        perimeter += sqrt(pow(next.x - current.x, 2) + pow(next.y - current.y, 2));
    }
    return perimeter;
}

double Polygon::get_area() 
{
    if(vertices_.size() < 3) 
    {
        return 0.0; // Not a polygon
    }
    double area = 0.0;
    for (std::size_t i = 0; i < vertices_.size(); ++i) 
    {
        const Point& current = vertices_[i];
        const Point& next = vertices_[(i + 1) % vertices_.size()];
        area += (current.x * next.y) - next.x * current.y;
    }
    return std::abs(area) / 2.0;
}

SampledGeometry Polygon::sample(double resolution) noexcept
{
    SampledGeometry sampled_geometry;
    // Implement sampling logic here, e.g., using a grid-based approach or random sampling.
    for (std::size_t i =0; i < vertices_.size(); ++i) 
    {
        Line edge(vertices_[i], vertices_[(i + 1) % vertices_.size()]);
        SampledGeometry edge_sampled = edge.sample(resolution);
        
        sampled_geometry.insert(sampled_geometry.end(), edge_sampled.begin(), edge_sampled.end());
    }
    return sampled_geometry;

}

}