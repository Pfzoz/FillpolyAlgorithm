#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include "scene.hh"
#include <string>
#include <SDL2/SDL_ttf.h>

class Button : public Component
{
private:
    int width, height;
    std::string text_content;
    SDL_Color fill_color = {0, 0, 0, 0};
    SDL_Color border_color = {0, 0, 0, 255};
    SDL_Color text_color = {0, 0, 0, 255};
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    TTF_Font *font = NULL;

    void create_texture(SDL_Renderer *renderer)
    {
        SDL_Surface *text_surface = TTF_RenderText_Solid(font, text_content.c_str(), text_color);
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        SDL_RenderFillRect(renderer, NULL);
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
        SDL_Rect btn_rect = {x, y, width, height};
        SDL_RenderDrawRect(renderer, &btn_rect);
        SDL_Rect dst_rect = {width / 2 - text_surface->w / 2, height / 2 - text_surface->h / 2, text_surface->w, text_surface->h};
        SDL_RenderCopy(renderer, text_texture, NULL, &dst_rect);
        SDL_SetRenderTarget(renderer, NULL);
        SDL_FreeSurface(text_surface);
        SDL_DestroyTexture(text_texture);
    }

public:
    bool ready = false;

    Button(std::string text_content, int width, int height, TTF_Font *font = NULL)
    {
        this->text_content = text_content;
        this->height = height;
        this->width = width;
        if (font == NULL)
        {
            font = TTF_OpenFont("../assets/fonts/open-sans/OpenSans-Regular.ttf", 14);
        }
        else
        {
            this->font = font;
        }
    }

    int get_width()
    {
        return width;
    }

    int get_height()
    {
        return height;
    }

    void set_dimensions(int width, int height)
    {
        this->width = width;
        this->height = height;
        if (ready)
        {
            SDL_DestroyTexture(texture);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            create_texture(renderer);
        }
    }

    void init(SDL_Renderer *renderer)
    {
        this->renderer = renderer;
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

    void background_fit(int x_offset = 0, int y_offset = 0)
    {
        SDL_Surface *text_surface = TTF_RenderText_Solid(font, text_content.c_str(), text_color);
        this->set_dimensions(text_surface->w + x_offset, text_surface->h + y_offset);
    }

    void fill(SDL_Color color)
    {
        this->fill_color = color;
        if (ready)
        {
            create_texture(renderer);
        }
    }

    void set_font(TTF_Font *font)
    {
        TTF_CloseFont(font);
        this->font = font;
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
        SDL_DestroyTexture(texture);
    }
};

#endif