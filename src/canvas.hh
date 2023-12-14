#ifndef CANVAS_H
#define CANVAS_H

#include <math.h>
#include "scene.hh"

class Canvas : public Component
{
private:
    SDL_Color border_color = {0, 0, 0, 255};

    void create_texture(SDL_Renderer *renderer)
    {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, x_res, y_res);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
        SDL_RenderDrawRect(renderer, NULL);
        SDL_SetRenderTarget(renderer, NULL);
    }

public:

    Canvas(float width, float height)
    {
        scaling = 64;
        this->set_geometry(0, 0, width, height);
    }

    void init(SDL_Renderer *renderer)
    {
        this->renderer = renderer;
        create_texture(renderer);
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_RenderCopy(renderer, texture, NULL, &geometry);
    }
};

#endif