#pragma once

#include <entity.hpp>
#include <SDL2/SDL.h>

namespace HBR::Simulator
{
    /// @brief Structure of world
class World
{
private:
  int length, width;
  const double g = -9.8;

public:
  void create_world(int ln, int wdt);
  void draw(SDL_Renderer * renderer);
  int get_length();
  int get_width();

protected:
};
}
