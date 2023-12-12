#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include "scene.hh"
#include <string>

class Button : public Component
{
private:
    int width, height;
    std::string text_content;
    SDL_Color fill_color = {0, 0, 0, 0};
    SDL_Color border_color = {0, 0, 0, 255};
    SDL_Texture *texture;
    SDL_Renderer *renderer;

public:
    bool ready = false;

    Button(std::string text_content, int width, int height)
    {
        this->text_content = text_content;
        this->height = height;
        this->width = width;
    }

    void init(SDL_Renderer *renderer)
    {
        this->renderer = renderer;
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        SDL_RenderFillRect(renderer, NULL);
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
        SDL_RenderDrawLine(renderer, x, y, x, y + height - 1);
        SDL_RenderDrawLine(renderer, x, y, x + width - 1, y);
        SDL_RenderDrawLine(renderer, x + width - 1, y, x + width - 1, y + height - 1);
        SDL_RenderDrawLine(renderer, x, y + height - 1, x + width - 1, y + height - 1);
        SDL_SetRenderTarget(renderer, NULL);
        ready = true;
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_Rect dst_rect = {x, y, width, height};
        SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
    }

    void fill(SDL_Color color)
    {
        this->fill_color = color;
        if (ready)
        {
            SDL_SetRenderTarget(renderer, texture);
            SDL_SetRenderDrawColor(renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
            SDL_RenderDrawRect(renderer, NULL);
            SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
            SDL_RenderDrawLine(renderer, x, y, x, y + height - 1);
            SDL_RenderDrawLine(renderer, x, y, x + width - 1, y);
            SDL_RenderDrawLine(renderer, x + width - 1, y, x + width - 1, y + height - 1);
            SDL_RenderDrawLine(renderer, x, y + height - 1, x + width - 1, y + height - 1);
            SDL_SetRenderTarget(renderer, NULL);
        }
    }

    bool in_bounds(int x, int y)
    {
        if (x >= this->x && y >= this->y && x <= (this->x + width) && y <= (this->y + height))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void destroy()
    {
    }
};

#endif