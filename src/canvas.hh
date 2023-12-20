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
    SDL_Color border_color = {0, 0, 0, 255};
    std::vector<Vertex *> vertices;
    std::vector<Edge *> edges, temp_edges;
    std::vector<Polygon *> polygons;

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
    int thickness = 3, current_vertices = 0;
    bool creating_polygon = false;

    Canvas(float width, float height)
    {
        scaling = 256;
        this->geometry.w = width;
        this->geometry.h = height;
        rescale(width, height);
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_RenderCopy(renderer, texture, NULL, &geometry);
        for (Vertex *vertex : vertices)
        {
            SDL_SetRenderDrawColor(renderer, vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a);
            for (int i = 0; i < vertex->thickness; i++)
            {
                // SDL_RenderDrawPoint(renderer, vertex->x * geometry.w + geometry.x, vertex->y * geometry.h + geometry.y);
                int vertex_x = vertex->x * geometry.w + geometry.x;
                int vertex_y = vertex->y * geometry.h + geometry.y;
                for (int j = vertex_x - i; j < (vertex_x + i); j++)
                {
                    SDL_RenderDrawPoint(renderer, j, vertex_y + i);
                }
                for (int j = vertex_x - i; j < (vertex_x + i); j++)
                {
                    SDL_RenderDrawPoint(renderer, j, vertex_y - i);
                }
                for (int j = vertex_y - i; j < (vertex_y + i); j++)
                {
                    SDL_RenderDrawPoint(renderer, vertex_x - i, j);
                }
                for (int j = vertex_y - i; j < (vertex_y + i); j++)
                {
                    SDL_RenderDrawPoint(renderer, vertex_x + i, j);
                }
            }
        }
        for (Edge *edge : edges)
        {
            SDL_SetRenderDrawColor(renderer, (edge->a->color.r + edge->b->color.r) / 2, (edge->a->color.g + edge->b->color.g) / 2, (edge->a->color.b + edge->b->color.b) / 2, (edge->a->color.a + edge->b->color.a) / 2);
            SDL_RenderDrawLine(renderer, edge->a->x * geometry.w + geometry.x, edge->a->y * geometry.h + geometry.y, edge->b->x * geometry.w + geometry.x, edge->b->y * geometry.h + geometry.y);
        }
        for (Polygon *polygon : polygons)
        {
            // scanline
        }
    }

    void open_polygon(float x, float y, SDL_Color color = {0, 0, 0, 255})
    {
        if (creating_polygon)
        {
            printf("Não se pode abrir um polígono quando um já está aberto.\n");
            std::exit(1);
        }
        this->creating_polygon = true;
        first = new Vertex((x - (float)geometry.x) / (float)geometry.w, (y - (float)geometry.y) / (float)geometry.h);
        first->color = color;
        first->thickness = thickness;
        this->vertices.push_back(first);
        current_vertices++;
    }

    void draw_polygon(float x, float y, SDL_Color color = {0, 0, 0, 255})
    {
        if (!creating_polygon)
        {
            printf("Não há polígonos sendo criados...\n");
            std::exit(1);
        }
        Vertex *vertex = new Vertex((x - (float)geometry.x) / (float)geometry.w, (y - (float)geometry.y) / (float)geometry.h);
        vertex->thickness = thickness;
        vertex->color = color;
        vertices.push_back(vertex);
        Edge *edge = new Edge(vertices[vertices.size() - 2], vertices[vertices.size() - 1]);
        edges.push_back(edge);
        temp_edges.push_back(edge);
        current_vertices++;
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
        current_vertices = 0;
    }

    Vertex *first_vertex_touched(float x, float y, float square_area = 0)
    {
        if (square_area <= 0)
        {
            for (Vertex *vertex : vertices)
            {
                float vertex_x = (vertex->x * (float)geometry.w) + (float)geometry.x;
                float vertex_y = (vertex->y * (float)geometry.h) + (float)geometry.y;
                if (x >= vertex_x - (float)vertex->thickness / 2 && y >= vertex_y - (float)vertex->thickness / 2 && x <= vertex_x + (float)vertex->thickness && y <= vertex_y + (float)vertex->thickness)
                    return vertex;
            }
        }
        else
        {
            for (Vertex *vertex : vertices)
            {
                float vertex_x = (vertex->x * (float)geometry.w) + (float)geometry.x;
                float vertex_y = (vertex->y * (float)geometry.h) + (float)geometry.y;
                if (x >= vertex_x - square_area / 2 && y >= vertex_y - square_area / 2 && x <= vertex_x + square_area && y <= vertex_y + square_area)
                    return vertex;
            }
        }
        return NULL;
    }
};

#endif