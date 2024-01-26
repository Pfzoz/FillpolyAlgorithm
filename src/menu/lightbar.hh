#ifndef LIGHTBAR_H
#define LIGHTBAR_H

#include <SDL2/SDL.h>
#include <string>
#include <algorithm>
#include <math.h>
#include "../scene/scene.hh"

class LightBar : public Component
{
private:
    float luminance = 0;
    SDL_Color border_color = {0, 0, 0, 255};

    void create_texture(SDL_Renderer *renderer)
    {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, x_res, y_res);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, texture);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
        SDL_RenderDrawRect(renderer, NULL);
        for (int y = 1; y < y_res - 1; y++)
        {
            float value = (255 / (float)y_res) * (float)y;
            SDL_SetRenderDrawColor(renderer, value, value, value, 255);
            SDL_RenderDrawLine(renderer, 1, y, x_res - 2, y);
        }
        SDL_SetRenderTarget(renderer, NULL);
    }

public:
    LightBar(float width, float height)
    {
        scaling = 8;
        geometry.w = width;
        geometry.h = height;
        rescale();
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_RenderCopy(renderer, texture, NULL, &geometry);
    }

    void set_luminance(int x, int y)
    {
        x = x - geometry.x;
        y = y - geometry.y;
        float sy = ((float)y_res - 2) / (float)geometry.h;
        luminance = (255 / (geometry.h*sy)) * y;
    }

    int get_luminance()
    {
        return luminance;
    }

    bool in_bounds(int x, int y)
    {
        return x >= geometry.x && y >= geometry.y && x <= (geometry.x + geometry.w) && y <= (geometry.y + geometry.h) ? true : false;
    }
};

#endif