#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include "../scene/scene.hh"

class Button : public Component
{
private:
    std::string text_content;
    SDL_Color fill_color = {0, 0, 0, 0};
    SDL_Color border_color = {0, 0, 0, 255};
    SDL_Color text_color = {0, 0, 0, 255};
    TTF_Font *font = NULL;

    void create_texture(SDL_Renderer *renderer)
    {
        TTF_SetFontSize(font, ptsize);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, x_res, y_res);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_Surface *text_surface = TTF_RenderUTF8_Solid(font, text_content.c_str(), text_color);
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        SDL_RenderFillRect(renderer, NULL);
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
        SDL_RenderDrawRect(renderer, NULL);
        SDL_Rect dst_rect = {x_res / 2 - text_surface->w / 2, y_res / 2 - text_surface->h / 2, text_surface->w, text_surface->h};
        SDL_RenderCopy(renderer, text_texture, NULL, &dst_rect);
        SDL_SetRenderTarget(renderer, NULL);
        SDL_FreeSurface(text_surface);
        SDL_DestroyTexture(text_texture);
    }

public:
    int ptsize = 22;

    Button(std::string text_content, float width, float height, TTF_Font *font)
    {
        scaling = 8;
        this->text_content = text_content;
        this->font = font;
        geometry.w = width;
        geometry.h = height;
        rescale(width, height);
    }

    void set_border_color(SDL_Color color)
    {
        this->border_color = color;
        this->reload(renderer);
    }

    int get_width()
    {
        return geometry.w;
    }

    int get_height()
    {
        return geometry.h;
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_RenderCopy(renderer, texture, NULL, &geometry);
    }

    void background_fit(int x_offset = 0, int y_offset = 0)
    {
        TTF_SetFontSize(font, ptsize);
        int new_width, new_height;
        TTF_SizeText(font, text_content.c_str(), &new_width, &new_height);
        set_dimensions(new_width + x_offset, new_height + y_offset);
    }

    void fill(SDL_Color color)
    {
        this->fill_color = color;
        if (texture != NULL)
        {
            SDL_DestroyTexture(texture);
            create_texture(renderer);
        }
    }

    void set_font(TTF_Font *font)
    {
        this->font = font;
    }

    bool in_bounds(int x, int y)
    {
        return x >= geometry.x && y >= geometry.y && x <= (geometry.x + geometry.w) && y <= (geometry.y + geometry.h) ? true : false;
    }
};

#endif