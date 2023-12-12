#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <SDL2/SDL.h>
#include <string>
#include "scene.hh"

class Vertex : public Component
{
public:
    SDL_Color color = {0, 0, 0, 255};

    Vertex(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawPoint(renderer, x, y);
    }

    void destroy() {}
};

#endif