#ifndef SCENE_H
#define SCENE_H

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
    void (*onclick_callback)(int x, int y) = [](int x, int y) {};
    void (*onright_click_callback)(int x, int y) = [](int x, int y) {};
    void (*onmiddle_click_callback)(int x, int y) = [](int x, int y) {};
    void (*onkeydown_callback)(SDL_Keycode sym) = [](SDL_Keycode sym) {};
    void (*ontextinput_callback)(SDL_TextInputEvent event, Component *target) = [](SDL_TextInputEvent event, Component *target) {};

    virtual void create_texture(SDL_Renderer *renderer){};

public:
    bool visible = true;
    bool focused = false;
    int x_res = 0, y_res = 0;
    float scaling = 32;
    SDL_Rect geometry = {0, 0, 0, 0};

    virtual void onclick(int x, int y)
    {
        if (in_bounds(x, y))
            this->onclick_callback(x, y);
    }

    virtual void onright_click(int x, int y)
    {
        if (in_bounds(x, y))
            this->onright_click_callback(x, y);
    }

    virtual void onmiddle_click(int x, int y)
    {
        if (in_bounds(x, y))
            this->onmiddle_click_callback(x, y);
    }

    virtual void onkeydown(SDL_Keycode sym)
    {
        this->onkeydown_callback(sym);
    }

    virtual void ontextinput(SDL_TextInputEvent event, Component *target)
    {
        this->ontextinput_callback(event, target);
    }

    virtual void set_ontextinput(void (*ontextinput_callback)(SDL_TextInputEvent event, Component *target))
    {
        this->ontextinput_callback = ontextinput_callback;
    }

    void set_onkeydown(void (*onkeydown_callback)(SDL_Keycode sym))
    {
        this->onkeydown_callback = onkeydown_callback;
    }

    void set_onclick(void (*onclick_callback)(int x, int y))
    {
        this->onclick_callback = onclick_callback;
    }

    void set_onright_click(void (*onright_click_callback)(int x, int y))
    {
        this->onright_click_callback = onright_click_callback;
    }

    void set_onmiddle_click(void (*onmiddle_click_callback)(int x, int y))
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
};

class Scene
{
private:
    std::vector<Component *> components;
    SDL_Renderer *renderer = NULL;
    Uint8 fill_colors[4];

public:
    SDL_Window *window = NULL;
    bool quit = false;
    void (*resize_callback)(int width, int height);

    Scene() {}
    Scene(SDL_Window *window)
    {
        this->window = window;
        this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        if (this->renderer == NULL)
        {
            printf("O renderizador não pode ser inicializado. Encerrando...\n");
            std::exit(1);
        }
    }

    void add_component(Component *component)
    {
        component->init(renderer);
        this->components.push_back(component);
    }

    void fill(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        fill_colors[0] = r;
        fill_colors[1] = g;
        fill_colors[2] = b;
        fill_colors[3] = a;
    }

    void fill(SDL_Color color)
    {
        fill_colors[0] = color.r;
        fill_colors[1] = color.g;
        fill_colors[2] = color.b;
        fill_colors[3] = color.a;
    }

    void render()
    {
        SDL_SetRenderDrawColor(renderer, fill_colors[0], fill_colors[1], fill_colors[2], fill_colors[3]);
        SDL_RenderClear(renderer);
        for (Component *component : components)
        {
            if (component->visible)
                component->draw(renderer);
        }
        SDL_RenderPresent(renderer);
    }

    void destroy(bool destroy_window = true)
    {
        for (int i = 0; i < components.size(); i++)
        {
            components[i]->destroy();
            delete components[i];
        }
        if (destroy_window)
        {
            if (window == NULL)
            {
                printf("Erro ao destruir a janela. Encerrando...\n");
                std::exit(1);
            }
            else
            {
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
            }
        }
    }

    void update()
    {
        for (Component *component : components)
            component->rescale(component->geometry.w, component->geometry.h);
    }

    void poll()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    for (Component *component : components)
                        component->onclick(event.button.x, event.button.y);
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    for (Component *component : components)
                        component->onright_click(event.button.x, event.button.y);
                }
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    for (Component *component : components)
                        component->onmiddle_click(event.button.x, event.button.y);
                }
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    resize_callback(event.window.data1, event.window.data2);
                    update();
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                for (Component *component : components)
                    component->onkeydown(event.key.keysym.sym);
            }
            else if (event.type == SDL_TEXTINPUT)
            {
                for (Component *component : components)
                    component->ontextinput(event.text, component);
            }
        }
        bool focus_presence = false;
        for (int i = 0; i < components.size() && !focus_presence; i++)
        {
            if (components[i]->focused)
                focus_presence = true;
        }
        if (focus_presence)
        {
            if (!SDL_IsTextInputActive())
                SDL_StartTextInput();
        }
        else if (SDL_IsTextInputActive())
            SDL_StopTextInput();
    }

    void onresize(void (*resize_callback)(int width, int height))
    {
        this->resize_callback = resize_callback;
    }

    void destroy(Component *component)
    {
        if (component != NULL)
        {
            component->destroy();
            delete component;
        }
        else
        {
            printf("Erro ao destroir um componente. Encerrando...\n");
            std::exit(1);
        }
    }
};

#endif