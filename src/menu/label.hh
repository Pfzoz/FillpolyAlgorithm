#ifndef LABEL_H
#define LABEL_H

#include <SDL2/SDL.h>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include "../scene/scene.hh"

class Label : public Component
{
private:
    int ptsize = 14;
    std::string text_content;
    TTF_Font *font;
    SDL_Color text_color = {0, 0, 0, 255};

    void create_texture(SDL_Renderer *renderer)
    {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, x_res, y_res);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        TTF_SetFontSize(font, ptsize);
        SDL_Surface *text_surface = TTF_RenderUTF8_Solid(font, text_content.c_str(), text_color);
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_Rect dst_rect = {geometry.w / 2 - text_surface->w / 2, geometry.h / 2 - text_surface->h / 2, text_surface->w, text_surface->h};
        SDL_RenderCopy(renderer, text_texture, NULL, &dst_rect);
        SDL_SetRenderTarget(renderer, NULL);
        SDL_FreeSurface(text_surface);
    }

public:
    Label(std::string text_content, float width, float height, TTF_Font *font)
    {
        scaling = 2;
        this->font = font;
        this->text_content = text_content;
        geometry.w = width;
        geometry.h = height;
        rescale(width, height);
    }

    Label(std::string text_content, TTF_Font* font, int ptsize)
    {
        scaling = 2;
        this->ptsize = ptsize;
        this->font = font;
        this->text_content = text_content;
        TTF_SetFontSize(font, ptsize);
        TTF_SizeText(font, text_content.c_str(), &geometry.w, &geometry.h);
        rescale(geometry.w, geometry.h);
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_RenderCopy(renderer, texture, NULL, &geometry);
    }

    void set_ptsize(int ptsize)
    {
        this->ptsize = ptsize;
        TTF_SetFontSize(font, ptsize);
        int new_width, new_height;
        TTF_SizeText(font, text_content.c_str(), &new_width, &new_height);
        set_dimensions(new_width, new_height);
    }

    void set_font(TTF_Font *font)
    {
        this->font = font;
    }

    void set_text_color(SDL_Color color)
    {
        this->text_color = color;
        reload(renderer);
    }

    void update_text(std::string text_content)
    {
        this->text_content = text_content;
        TTF_SetFontSize(font, ptsize);
        TTF_SizeText(font, text_content.c_str(), &geometry.w, &geometry.h);
        rescale(geometry.w, geometry.h);
        reload(renderer);
    }
};

#endif