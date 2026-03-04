#pragma once

#include <entity.hpp>
#include <vec2d.hpp>

namespace HBR::Simulator
{
    /// @brief Structure of Robot
class Robot : public Entity
{
private:
public:
    int id;
    const char *name;
    int length, width;
    std::vector<int> color{0, 0, 0, 255}; //{R,G,B,A}

    Robot() {};
    Robot(int idd, char *nm, int l, int w, std::vector<int> clr);

    ~Robot();

protected:
};

}
