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

    virtual void create_texture(SDL_Renderer *renderer){};

public:
    int x_res = 0, y_res = 0;
    int scaling = 32;
    SDL_Rect geometry;

    Component() {}
    virtual void init(SDL_Renderer *renderer)
    {
        this->renderer = renderer;
        create_texture(renderer);
    }
    virtual void draw(SDL_Renderer *renderer){};
    virtual void set_geometry(int x, int y, float width, float height)
    {
        geometry.x = x;
        geometry.y = y;
        geometry.w = width;
        geometry.h = height;
        int new_x_res = round((width / scaling)) * scaling;
        int new_y_res = round((height / scaling)) * scaling;
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
    virtual void set_position(int x, int y)
    {
        geometry.x = x;
        geometry.y = y;
    }
    virtual void set_dimensions(float width, float height)
    {
        geometry.w = width;
        geometry.h = height;
        int new_x_res = round((width / scaling)) * scaling;
        int new_y_res = round((height / scaling)) * scaling;
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
    virtual void destroy()
    {
        SDL_DestroyTexture(texture);
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
    Scene() {}
    Scene(SDL_Window *window)
    {
        this->window = window;
        this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        int h, w;
        SDL_GetWindowSize(window, &w, &h);
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
        for (int i = 0; i < components.size(); i++)
        {
            components[i]->draw(renderer);
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