#ifndef CANVAS_H
#define CANVAS_H

#include "scene.hh"

class Canvas : public Component
{
private:
    int destroys = 0, creates = 0;
    int width, height;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    SDL_Color border_color = {0, 0, 0, 255};

    void create_texture(SDL_Renderer *renderer)
    {
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
        SDL_RenderDrawRect(renderer, NULL);
        SDL_SetRenderTarget(renderer, NULL);
    }

public:
    bool ready = false;

    Canvas(int width, int height)
    {
        this->width = width;
        this->height = height;
    }

    void init(SDL_Renderer *renderer)
    {
        this->renderer = renderer;
        creates++;
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        create_texture(renderer);
        ready = true;
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_Rect dst_rect = {x, y, width, height};
        SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
    }

    void set_dimensions(int width, int height)
    {
        this->width = width;
        this->height = height;
        if (ready)
        {
            destroys++;
            SDL_DestroyTexture(texture);
            creates++;
            printf("Creates: %i Destroys: %i\n", creates, destroys);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            create_texture(renderer);
        }
    }
};

#endif