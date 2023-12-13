#ifndef CANVAS_H
#define CANVAS_H

#include <math.h>
#include "scene.hh"

class Canvas : public Component
{
private:
    const int scaling = 128;
    int x_res = 0, y_res = 0;
    SDL_Rect geometry;
    SDL_Texture *texture = NULL;
    SDL_Renderer *renderer = NULL;
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

    void set_geometry(int x, int y, float width, float height)
    {
        geometry.x = x;
        geometry.y = y;
        geometry.w = width;
        geometry.h = height;
        int new_x_res = round((width / scaling)) * scaling;
        int new_y_res = round((height / scaling)) * scaling;
        if (new_x_res != x_res || new_y_res != y_res)
        {
            x_res = new_x_res;
            y_res = new_y_res;
            if (texture != NULL)
            {
                SDL_DestroyTexture(texture);
                create_texture(renderer);
            }
        }
    }
};

#endif