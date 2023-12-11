#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

class Component
{
public:
    int x = 0, y = 0;
    Component() {}
    virtual void draw(SDL_Renderer *renderer){};
    virtual void destroy(){};
};

class Scene
{
private:
    std::vector<Component *> components;
    SDL_Renderer *renderer;
    Uint8 fill_colors[4];

public:
    SDL_Window *window;
    Scene() {}
    Scene(SDL_Window *window)
    {
        this->window = window;
        this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (this->renderer == NULL)
        {
            printf("O renderizador não pode ser inicializado. Encerrando...\n");
            std::exit(1);
        }
    }

    void add_component(Component *component)
    {
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
        SDL_Surface *surface = SDL_GetWindowSurface(window);
        SDL_SetRenderDrawColor(renderer, fill_colors[0], fill_colors[1], fill_colors[2], fill_colors[3]);
        SDL_RenderFillRect(renderer, NULL);
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