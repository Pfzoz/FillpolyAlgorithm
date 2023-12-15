#ifndef CANVAS_H
#define CANVAS_H

#include <SDL2/SDL.h>
#include <math.h>
#include <vector>
#include "scene.hh"
#include "geometry.hh"

class Canvas : public Component
{
private:
    Vertex *first = NULL;
    bool creating_polygon = false;
    SDL_Color border_color = {0, 0, 0, 255};
    std::vector<Vertex *> vertices;
    std::vector<Edge *> edges;
    std::vector<Polygon *> polygons;
    std::vector<Edge *> temp_edges;

    void create_texture(SDL_Renderer *renderer)
    {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, x_res, y_res);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
        SDL_RenderDrawRect(renderer, NULL);
        SDL_SetRenderTarget(renderer, NULL);
    }

public:
    int thickness = 1;

    Canvas(float width, float height)
    {
        scaling = 64;
        this->set_geometry(0, 0, width, height);
    }

    void init(SDL_Renderer *renderer)
    {
        this->renderer = renderer;
        create_texture(renderer);
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_RenderCopy(renderer, texture, NULL, &geometry);
        for (Vertex *vertex : vertices)
        {
            SDL_SetRenderDrawColor(renderer, vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a);
            for (int i = 0; i < vertex->thickness; i++)
            {
                for (int j = vertex->x - i; j < (vertex->x + i); j++)
                {
                    SDL_RenderDrawPoint(renderer, j + geometry.x, vertex->y + i + geometry.y);
                }
                for (int j = vertex->x - i; j < (vertex->x + i); j++)
                {
                    SDL_RenderDrawPoint(renderer, j + geometry.x, vertex->y - i + geometry.y);
                }
                for (int j = vertex->y - i; j < (vertex->y + i); j++)
                {
                    SDL_RenderDrawPoint(renderer, vertex->x - i + geometry.x, j + geometry.y);
                }
                for (int j = vertex->y - i; j < (vertex->y + i); j++)
                {
                    SDL_RenderDrawPoint(renderer, vertex->x + i + geometry.x, j + geometry.y);
                }
            }
        }
        for (Edge *edge : edges)
        {
            SDL_SetRenderDrawColor(renderer, (edge->a->color.r + edge->b->color.r) / 2, (edge->a->color.g + edge->b->color.g) / 2, (edge->a->color.b + edge->b->color.b) / 2, (edge->a->color.a + edge->b->color.a) / 2);
            SDL_RenderDrawLine(renderer, edge->a->x + geometry.x, edge->a->y + geometry.y, edge->b->x + geometry.x, edge->b->y + geometry.y);
        }
        for (Polygon *polygon : polygons)
        {
            // scanline
        }
    }

    void set_dimensions(float width, float height)
    {
        int old_width = geometry.w;
        int old_height = geometry.h;
        geometry.w = width;
        geometry.h = height;
        int new_x_res = round((width / scaling)) * scaling;
        int new_y_res = round((height / scaling)) * scaling;
        for (Vertex *vertex : vertices)
        {
            int new_x = (int) (((float) vertex->x / (float) old_width) * width);
            int new_y = (int) (((float) vertex->y / (float) old_height) * height);
            vertex->x = new_x;
            vertex->y = new_y;
        }
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

    void open_polygon(int x, int y, SDL_Color color = {0,0,0,255})
    {
        if (creating_polygon)
        {
            printf("Não se pode abrir um polígono quando um já está aberto.\n");
            std::exit(1);
        }
        this->creating_polygon = true;
        first = new Vertex(x - geometry.x, y - geometry.y);
        first->color = color;
        first->thickness = thickness;
        this->vertices.push_back(first);
    }

    void draw_polygon(int x, int y, SDL_Color color = {0,0,0,255})
    {
        if (!creating_polygon)
        {
            printf("Não há polígonos sendo criados...\n");
            std::exit(1);
        }
        Vertex *vertex = new Vertex(x - geometry.x, y - geometry.y);
        vertex->thickness = thickness;
        vertex->color = color;
        vertices.push_back(vertex);
        Edge *edge = new Edge(vertices[vertices.size() - 2], vertices[vertices.size() - 1]);
        edges.push_back(edge);
        temp_edges.push_back(edge);
    }

    void close_polygon()
    {
        if (!creating_polygon)
        {
            printf("Não há polígonos sendo criados...\n");
            std::exit(1);
        }
        Edge *edge = new Edge(vertices[vertices.size() - 1], first);
        first = NULL;
        edges.push_back(edge);
        temp_edges.push_back(edge);
        if (temp_edges.size() < 3)
        {
            printf("É necessário mais de 2 arestas para criar um polígono.\n");
            std::exit(1);
        }
        Polygon *polygon = new Polygon();
        for (Edge *edge : temp_edges)
        {
            polygon->edges.push_back(edge);
        }
        temp_edges.clear();
        polygons.push_back(polygon);
        creating_polygon = false;
    }

    bool in_bounds(int x, int y)
    {
        return x >= geometry.x && y >= geometry.y && x <= (geometry.x + geometry.w) && y <= (geometry.y + geometry.h) ? true : false;
    }
};

#endif