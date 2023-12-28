#ifndef COMPONENT_H
#define COMPONENT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>
#include <algorithm>

class Component
{
protected:
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    std::string text_content = "";
    bool visible = true;
    void (*onmotion_callback)(int x, int y, bool hit) = [](int x, int y, bool hit) {};
    void (*onclick_callback)(int x, int y, bool hit) = [](int x, int y, bool hit) {};
    void (*onright_click_callback)(int x, int y, bool hit) = [](int x, int y, bool hit) {};
    void (*onmiddle_click_callback)(int x, int y, bool hit) = [](int x, int y, bool hit) {};
    void (*onkeydown_callback)(SDL_Keycode sym) = [](SDL_Keycode sym) {};
    void (*ontextinput_callback)(SDL_TextInputEvent event, Component *target) = [](SDL_TextInputEvent event, Component *target) {};

    virtual void create_texture(SDL_Renderer *renderer){};

public:
    int z_index = 0;
    bool focused = false;
    int x_res = 0, y_res = 0;
    float scaling = 32;
    SDL_Rect geometry = {0, 0, 0, 0};

    virtual void onmotion(int x, int y, bool hit = true)
    {
        onmotion_callback(x, y, hit);
    }

    virtual void onclick(int x, int y, bool hit = true)
    {
        this->onclick_callback(x, y, hit);
    }

    virtual void onright_click(int x, int y, bool hit = true)
    {
        if (hit)
            this->onright_click_callback(x, y, hit);
        else
            this->onright_click_callback(x, y, hit);
    }

    virtual void onmiddle_click(int x, int y, bool hit = true)
    {
        if (hit)
            this->onmiddle_click_callback(x, y, hit);
        else
            this->onmiddle_click_callback(x, y, hit);
    }

    virtual void onkeydown(SDL_Keycode sym)
    {
        this->onkeydown_callback(sym);
    }

    virtual void ontextinput(SDL_TextInputEvent event, Component *target)
    {
        this->ontextinput_callback(event, target);
    }

    virtual void set_onmotion(void (*onmotion_callback)(int x, int y, bool hit))
    {
        this->onmotion_callback = onmotion_callback;
    }

    virtual void set_ontextinput(void (*ontextinput_callback)(SDL_TextInputEvent event, Component *target))
    {
        this->ontextinput_callback = ontextinput_callback;
    }

    void set_onkeydown(void (*onkeydown_callback)(SDL_Keycode sym))
    {
        this->onkeydown_callback = onkeydown_callback;
    }

    void set_onclick(void (*onclick_callback)(int x, int y, bool hit))
    {
        this->onclick_callback = onclick_callback;
    }

    void set_onright_click(void (*onright_click_callback)(int x, int y, bool hit))
    {
        this->onright_click_callback = onright_click_callback;
    }

    void set_onmiddle_click(void (*onmiddle_click_callback)(int x, int y, bool hit))
    {
        this->onmiddle_click_callback = onmiddle_click_callback;
    }

    Component() {}

    virtual void init(SDL_Renderer *renderer)
    {
        this->renderer = renderer;
        if (texture == NULL)
            create_texture(renderer);
        else
        {
            printf("Error during component initialization\n");
            std::exit(1);
        }
    }
    virtual void reload(SDL_Renderer *renderer)
    {
        if (texture != NULL)
        {
            SDL_DestroyTexture(texture);
            create_texture(renderer);
        }
    }
    virtual void set_visible(bool visibility)
    {
        bool was_visible = visible;
        visible = visibility;
        if (visible && !was_visible)
            reload(renderer);
    }
    virtual void draw(SDL_Renderer *renderer){};
    virtual void set_geometry(int x, int y, float width, float height)
    {
        set_position(x, y);
        set_dimensions(width, height);
    }
    virtual void set_position(int x, int y)
    {
        geometry.x = x;
        geometry.y = y;
    }
    virtual void set_dimensions(float width, float height)
    {
        geometry.w = width;
        geometry.h = height;
    }
    virtual void rescale(float width, float height)
    {
        int new_x_res = ceil((width / scaling)) * scaling;
        int new_y_res = ceil((height / scaling)) * scaling;
        if (new_x_res != x_res || new_y_res != y_res)
        {
            x_res = std::max<int>(new_x_res, scaling);
            y_res = std::max<int>(new_y_res, scaling);
            if (texture != NULL)
            {
                SDL_DestroyTexture(texture);
                create_texture(renderer);
            }
        }
    }
    virtual void rescale()
    {
        rescale(geometry.w, geometry.h);
    }
    virtual bool in_bounds(int x, int y)
    {
        return x >= geometry.x && y >= geometry.y && x <= (geometry.x + geometry.w) && y <= (geometry.y + geometry.h) ? true : false;
    }
    virtual void destroy()
    {
        SDL_DestroyTexture(texture);
    }
    virtual void update_text(std::string text)
    {
        this->text_content = text;
        this->reload(renderer);
    }
    virtual bool is_visible() { return visible; }
};

class ComponentCollection
{
private:
public:
    std::vector<Component *> components;

    virtual void update_geometry(){};
};

#endif