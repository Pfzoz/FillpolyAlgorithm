#ifndef DIALOG_H
#define DIALOG_H

#include <SDL2/SDL.h>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include "../scene.hh"

class DialogBox : public Component
{
private:
    int ptsize = 14;
    std::string default_text = "";
    TTF_Font *font;
    SDL_Color fill_color = {0, 0, 0, 0};
    SDL_Color border_color = {0, 0, 0, 255};
    SDL_Color text_color = {0, 0, 0, 255};

    void create_texture(SDL_Renderer *renderer)
    {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, x_res, y_res);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, texture);
        if (text_content != "")
        {
            TTF_SetFontSize(font, ptsize);
            SDL_Surface *text_surface;
            if (focused)
            {
                std::string writing_string = text_content + "_";
                text_surface = TTF_RenderUTF8_Solid(font, writing_string.c_str(), text_color);
            }
            else
                text_surface = TTF_RenderUTF8_Solid(font, text_content.c_str(), text_color);
            SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
            SDL_RenderFillRect(renderer, NULL);
            if (focused)
                SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a/2);
            else
                SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
            SDL_RenderDrawRect(renderer, NULL);
            SDL_Rect dst_rect = {x_res / 2 - text_surface->w / 2, y_res / 2 - text_surface->h / 2, text_surface->w, text_surface->h};
            SDL_RenderCopy(renderer, text_texture, NULL, &dst_rect);
            SDL_FreeSurface(text_surface);
            SDL_DestroyTexture(text_texture);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
            SDL_RenderFillRect(renderer, NULL);
            if (focused)
            {
                SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a/2);
                SDL_RenderDrawRect(renderer, NULL);
                TTF_SetFontSize(font, ptsize);
                SDL_Surface *text_surface = TTF_RenderUTF8_Solid(font, "_", text_color);
                SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
                SDL_Rect dst_rect = {x_res / 2 - text_surface->w / 2, y_res / 2 - text_surface->h / 2, text_surface->w, text_surface->h};
                SDL_RenderCopy(renderer, text_texture, NULL, &dst_rect);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
                SDL_RenderDrawRect(renderer, NULL);
            }
        }
        SDL_SetRenderTarget(renderer, NULL);
    }

public:
    bool digits_only = false;

    DialogBox(std::string text_content, int ptsize, TTF_Font *font, float width, float height)
    {
        scaling = 2;
        this->font = font;
        this->ptsize = ptsize;
        this->text_content = text_content;
        this->geometry.w = width;
        this->geometry.h = height;
        rescale(width, height);
    }

    DialogBox(std::string text_content, int ptsize, TTF_Font *font)
    {
        scaling = 2;
        this->font = font;
        this->ptsize = ptsize;
        this->text_content = text_content;
        TTF_SizeText(font, text_content.c_str(), &geometry.w, &geometry.h);
        rescale(geometry.w, geometry.h);
    }
    
    void update_text(std::string text_content)
    {
        this->text_content = text_content;
        reload(renderer);
    }

    void set_ptsize(int ptsize)
    {
        this->ptsize = ptsize;
        reload(renderer);
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_RenderCopy(renderer, texture, NULL, &geometry);
    }

    void set_font(TTF_Font *font)
    {
        this->font = font;
    }

    std::string get_text_content()
    {
        return text_content;
    }

    bool in_bounds(int x, int y)
    {
        return x >= geometry.x && y >= geometry.y && x <= (geometry.x + geometry.w) && y <= (geometry.y + geometry.h) ? true : false;
    }

    void set_default_text(std::string text)
    {
        if (text_content == default_text)
        {
            default_text = text;
            update_text(text);
        }
        else
            default_text = text;
    }

    void onkeydown(SDL_Keycode sym)
    {
        if (focused)
        {
            if (sym == SDLK_BACKSPACE && text_content.length() > 0)
            {
                if (text_content.length() == 1)
                    text_content = default_text;
                else
                    text_content.pop_back();
                reload(renderer);
            }
        }
        this->onkeydown_callback(sym);
    }

    void onclick(int x, int y, bool hit = true)
    {
        if (hit)
        {
            focused = !focused;
            this->onclick_callback(x, y, hit);
            reload(renderer);
        }
        else 
        {
            focused = false;
            this->onclick_callback(x, y, hit);
            reload(renderer);
        }

    }

    bool is_visible() { return visible; }

    void set_visible(bool visibility)
    {
        visible = visibility;
        if (!visible)
            focused = false;
            reload(renderer);
    }


    void ontextinput(SDL_TextInputEvent event, Component *target)
    {
        if (focused)
        {
            if (default_text == text_content)
            {
                text_content = "";
            }
            if (digits_only && std::isdigit(event.text[0]))
            {
                text_content.push_back(event.text[0]);
                reload(renderer);
                this->ontextinput_callback(event, target);
            }
            else if (!digits_only)
            {
                text_content.push_back(event.text[0]);
                reload(renderer);
                this->ontextinput_callback(event, target);
            }
        }
    }
};

#endif