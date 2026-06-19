#pragma once

#include <entity.hpp>


namespace HBR::Simulator
{
    /// @brief Structure of Obstacle
class Obstacle : public Entity
{
private:
            // int x_, y_;
  std::vector<int> color{0, 0, 0, 255};           //{R,G,B,A}

public:
  Obstacle() = default;
            // bool create_entity(int mass, Geometry::Point location, bool can_move = false, std::vector<int> clr={0, 0, 0, 255}) noexcept override;
};
}
