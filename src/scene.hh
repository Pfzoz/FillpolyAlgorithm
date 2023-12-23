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
    bool visible = true;
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

    virtual void onclick(int x, int y, bool hit = true)
    {
        if (hit)
            this->onclick_callback(x, y, hit);
        else
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

class ComponentCollection {
private:

public:
    std::vector<Component *> components;

    virtual void update_geometry() {};
};

class Scene
{
private:
    std::vector<std::vector<Component *>> components;
    int max_z = 0;
    SDL_Renderer *renderer = NULL;
    Uint8 fill_colors[4];

public:
    SDL_Window *window = NULL;
    bool quit = false;
    void (*resize_callback)(int width, int height);
    void (*onclick_callback)(int x, int y) = [](int x, int y) {};

    Scene() {}
    Scene(SDL_Window *window)
    {
        this->window = window;
        this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        components.push_back(std::vector<Component *>{});
        if (this->renderer == NULL)
        {
            printf("O renderizador não pode ser inicializado. Encerrando...\n");
            std::exit(1);
        }
    }

    void add_collection(ComponentCollection *collection)
    {
        for (Component *component : collection->components)
            add_component(component);
    }

    void add_component(Component *component)
    {
        component->init(renderer);
        if (component->z_index > max_z)
        {
            max_z = component->z_index;
            while (components.size() != max_z + 1)
            {
                components.push_back(std::vector<Component *>{});
            }
        }
        this->components[component->z_index].push_back(component);
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
        for (int i = max_z; i > -1; i--)
        {
            for (Component *component : components[i])
            {
                if (component->is_visible())
                    component->draw(renderer);
            }
        }
        SDL_RenderPresent(renderer);
    }

    Component *get_first_element(int x, int y)
    {
        for (int i = max_z; i > -1; i--)
        {
            for (Component *component : components[i])
            {
                if (component->in_bounds(x, y) && component->is_visible())
                    return component;
            }
        }
        return NULL;
    }

    void destroy(bool destroy_window = true)
    {
        for (int i = 0; i < components.size(); i++)
        {
            for (int j = 0; j < components[i].size(); j++)
            {
                components[i][j]->destroy();
                delete components[i][j];
            }
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
        for (int i = 0; i < components.size(); i++)
        {
            for (Component *component : components[i])
                component->rescale(component->geometry.w, component->geometry.h);
        }
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
                    onclick_callback(event.button.x, event.button.y);
                    Component *first_hit = get_first_element(event.button.x, event.button.y);
                    if (first_hit != NULL)
                        first_hit->onclick(event.button.x, event.button.y);
                    for (int i = 0; i < components.size(); i++)
                    {
                        for (int j = 0; j < components[i].size(); j++)
                        {
                            if (components[i][j] != first_hit)
                                components[i][j]->onclick(event.button.x, event.button.y, false);
                        }
                    }
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    Component *first_hit = get_first_element(event.button.x, event.button.y);
                    if (first_hit != NULL)
                        first_hit->onright_click(event.button.x, event.button.y);
                    for (int i = 0; i < components.size(); i++)
                    {
                        for (int j = 0; j > components[i].size(); j++)
                        {
                            if (components[i][j] != first_hit)
                                components[i][j]->onright_click(event.button.x, event.button.y, false);
                        }
                    }
                }
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    Component *first_hit = get_first_element(event.button.x, event.button.y);
                    if (first_hit != NULL)
                        first_hit->onmiddle_click(event.button.x, event.button.y);
                    for (int i = 0; i < components.size(); i++)
                    {
                        for (int j = 0; j > components[i].size(); j++)
                        {
                            if (components[i][j] != first_hit)
                                components[i][j]->onmiddle_click(event.button.x, event.button.y, false);
                        }
                    }
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
                for (int i = 0; i < components.size(); i++)
                {
                    for (Component *component : components[i])
                        component->onkeydown(event.key.keysym.sym);
                }
            }
            else if (event.type == SDL_TEXTINPUT)
            {
                for (int i = 0; i < components.size(); i++)
                {
                    for (Component *component : components[i])
                        component->ontextinput(event.text, component);
                }
            }
        }
        bool focus_presence = false;
        for (int j = 0; j < components.size() && !focus_presence; j++)
        {
            for (int i = 0; i < components[j].size() && !focus_presence; i++)
            {
                if (components[j][i]->focused)
                    focus_presence = true;
            }
        }
        if (focus_presence)
        {
            if (!SDL_IsTextInputActive())
                SDL_StartTextInput();
        }
        else if (SDL_IsTextInputActive())
            SDL_StopTextInput();
    }

    void onclick(void (*onclick_callback)(int x, int y))
    {
        this->onclick_callback = onclick_callback;
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