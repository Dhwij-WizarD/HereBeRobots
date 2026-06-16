#include <world.hpp>
#include <iostream>
namespace HBR::Simulator
{
    void World::create_world(int ln, int wdt)
    {
        length = ln;
        width = wdt;
    }

    void World::draw(SDL_Renderer* renderer)
    {
        // Draw world background
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_Rect background = {0, 0, length, width};
        SDL_RenderFillRect(renderer, &background);

        // Draw a simple test square in center
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect rect = {length / 2 - 50, width / 2 - 50, 100, 100};
        SDL_RenderFillRect(renderer, &rect);
    }

    int World::get_length()
    {
        return length;
    }

    int World::get_width()
    {
        return width;
    }
}