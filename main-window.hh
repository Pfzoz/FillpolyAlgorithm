#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "geometry.hh"
#include <string>

class Component
{

public:
    int x = 0, y = 0, width, height;
    bool hidden = false;
    SDL_Texture *texture;

    Component(SDL_Renderer *renderer, std::string path)
    {
        SDL_Surface *surface = IMG_Load(path.c_str());
        if (surface == NULL)
        {
            printf("Erro ao inicializar o componente: %s\n", path.c_str());
            std::exit(1);
        }
        width = surface->w;
        height = surface->h;
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == NULL)
        {
            printf("Erro ao inicializar a texture do componente: %s\n", path.c_str());
            std::exit(1);
        }
        SDL_FreeSurface(surface);
    }

    Component(SDL_Renderer *renderer, SDL_Surface *surface, bool free_surface = true)
    {
        if (surface == NULL)
        {
            printf("Erro ao carregar texture de componente-surface: surface is null\n");
            std::exit(1);
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        this->width = surface->w;
        this->height = surface->h;
        if (texture == NULL)
        {
            printf("Erro ao inicializar a textura do componente-surface.\n");
            std::exit(1);
        }
        if (free_surface)
            SDL_FreeSurface(surface);
    }

    void setTexture(SDL_Renderer *renderer, SDL_Surface *surface, bool free_texture = true, bool free_surface = true, bool fit = true)
    {
        if (free_texture)
            SDL_DestroyTexture(texture);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (fit)
        {
            width = surface->w;
            height = surface->h;
        }
        if (free_surface)
            SDL_FreeSurface(surface);
    }

    void setPosition(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    bool in_bounds(int x, int y)
    {
        if (x >= this->x && y >= this->y && x <= this->x + this->width && y <= this->y + this->height)
        {
            return true;
        }
        return false;
    }

    bool touches(int x, int y)
    {
        return true;
    }
};

class MainWindow
{
private:
    char point_label_serializer = 'A';
    std::vector<Point> points;
    std::vector<Triangle> triangles;
    std::vector<Component *> components;
    int triangle_point_count = 0;
    Uint8 r = 255, g = 255, b = 255, a = 255;

    void fill()
    {
        SDL_SetRenderDrawColor(sdl_renderer, r, g, b, a);
        SDL_RenderFillRect(sdl_renderer, NULL);
    }

public:
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;

    MainWindow(int width, int height)
    {
        sdl_window = SDL_CreateWindow("SDL Fillpoly", 0, 0, width, height, SDL_WINDOW_RESIZABLE);
        if (sdl_window == NULL)
        {
            printf("Erro ao criar a janela principal. Encerrando...\n");
            std::exit(1);
        }
        sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
    }

    void setFillColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    void render()
    {
        SDL_RenderClear(sdl_renderer);
        fill();
        for (int i = 0; i < components.size(); i++)
        {
            if (!components[i]->hidden)
            {
                SDL_Rect location;
                location.x = components[i]->x;
                location.y = components[i]->y;
                location.w = components[i]->width;
                location.h = components[i]->height;
                SDL_RenderCopy(sdl_renderer, components[i]->texture, NULL, &location);
            }
        }
        for (int i = 0; i < triangles.size(); i++)
        {
            SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
            SDL_RenderDrawLine(sdl_renderer, triangles[i].m.j.x, triangles[i].m.j.y, triangles[i].m.k.x, triangles[i].m.k.y);
            SDL_RenderDrawLine(sdl_renderer, triangles[i].n.j.x, triangles[i].n.j.y, triangles[i].n.k.x, triangles[i].n.k.y);
            SDL_RenderDrawLine(sdl_renderer, triangles[i].o.j.x, triangles[i].o.j.y, triangles[i].o.k.x, triangles[i].o.k.y);
        }
        for (int i = 0; i < points.size(); i++)
        {
            SDL_SetRenderDrawColor(sdl_renderer, points[i].r, points[i].g, points[i].b, points[i].a);
            SDL_RenderDrawPoint(sdl_renderer, points[i].x, points[i].y);
        }
        SDL_RenderPresent(sdl_renderer);
    }

    void destroy()
    {
        for (int i = 0; i < components.size(); i++)
        {
            SDL_DestroyTexture(components[i]->texture);
            delete components[i];
        }
        SDL_DestroyWindow(sdl_window);
        SDL_DestroyRenderer(sdl_renderer);
    }

    void draw_component(Component *component)
    {
        this->components.push_back(component);
    }

    // Geometry

    void draw_triangle(Point a, Point b, Point c, std::string label)
    {
        Triangle triangle(a, b, c);
        this->triangles.push_back(triangle);
    }

    void draw_point(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a, std::string label)
    {
        Point point{label, x, y, r, g, b, a};
        points.push_back(point);
    }

    void clear_points()
    {
        points.clear();
    }
};

#endif