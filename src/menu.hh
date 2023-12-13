#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include "scene.hh"
#include <string>
#include <SDL2/SDL_ttf.h>

class Button : public Component
{
private:
    const int scaling = 2;
    int x_res = 0, y_res = 0;
    std::string text_content;
    SDL_Color fill_color = {0, 0, 0, 0};
    SDL_Color border_color = {0, 0, 0, 255};
    SDL_Color text_color = {0, 0, 0, 255};
    SDL_Texture *texture = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font = NULL;

    void create_texture(SDL_Renderer *renderer)
    {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, x_res, y_res);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_Surface *text_surface = TTF_RenderText_Solid(font, text_content.c_str(), text_color);
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
    Button(std::string text_content, float width, float height, TTF_Font *font)
    {
        this->text_content = text_content;
        set_geometry(0, 0, width, height);
        this->font = font;
    }

    int get_width()
    {
        return geometry.w;
    }

    int get_height()
    {
        return geometry.h;
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

    void set_dimensions(float width, float height)
    {
        geometry.w = width;
        geometry.h = height;
        int new_x_res = round((width / scaling)) * scaling;
        int new_y_res = round((height / scaling)) * scaling;
        if (new_x_res != x_res || new_y_res != y_res)
        {
            this->x_res = new_x_res;
            this->y_res = new_y_res;
            if (texture != NULL)
            {
                SDL_DestroyTexture(texture);
                create_texture(renderer);
            }
        }
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

    void background_fit(int x_offset = 0, int y_offset = 0)
    {
        SDL_Surface *text_surface = TTF_RenderText_Solid(font, text_content.c_str(), text_color);
        set_dimensions(text_surface->w + x_offset, text_surface->h + y_offset);
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

    void destroy()
    {
        SDL_DestroyTexture(texture);
    }
};

#endif