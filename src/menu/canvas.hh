#ifndef CANVAS_H
#define CANVAS_H

#include <SDL2/SDL.h>
#include <math.h>
#include <vector>
#include "../scene/scene.hh"
#include "../geometry.hh"

class Canvas : public Component
{
    SDL_Color border_color = {0, 0, 0, 255};
    SDL_Color fill_color = {255, 255, 255, 255};

private:
    Vertex *first = NULL;
    std::vector<Vertex *> vertices;
    std::vector<Edge *> edges, temp_edges;
    std::vector<Polygon *> polygons;

    void draw_vertices()
    {
        for (Vertex *vertex : vertices)
        {
            SDL_SetRenderDrawColor(renderer, vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a);
            std::vector<SDL_Point> points;
            points.push_back({vertex->x, vertex->y});
            for (int i = 0; i < vertex->thickness; i++)
            {
                for (int j = vertex->x - i; j < (vertex->x + i); j++)
                {
                    points.push_back({j, vertex->y + i});
                    points.push_back({j, vertex->y - i});
                }
                for (int j = vertex->y - i; j < (vertex->y + i); j++)
                {
                    points.push_back({vertex->x - i, j});
                    points.push_back({vertex->x + i, j});
                }
            }
            SDL_RenderDrawPoints(renderer, points.data(), points.size());
        }
    }

    struct Active_Edge
    {
        float y_max, x_y_min;
        float m_inversed;
        Edge *edge;
    };

    bool static table_comparison(Active_Edge x, Active_Edge y)
    {
        return (x.x_y_min < y.x_y_min);
    }

    int rgb_clamp(float x)
    {
        if (x < 0)
            return 0;
        else if (x > 255)
            return 255;
        return x;
    }

    // Scanline Algorithm
    void scanline_fill()
    {
        for (Polygon *polygon : polygons)
        {
            std::vector<Active_Edge> active_table;
            std::vector<Edge *> _temp_edges = polygon->edges;
            for (int y = polygon->y_min; y < polygon->y_max; y++)
            {
                // Interception
                for (int i = 0; i < _temp_edges.size(); i++)
                {
                    if (y == _temp_edges[i]->y_min)
                    {
                        active_table.push_back({(float)_temp_edges[i]->y_max, (float)_temp_edges[i]->x_y_min, _temp_edges[i]->m_inversed, _temp_edges[i]});
                        _temp_edges.erase(_temp_edges.begin() + i--);
                    }
                }
                for (int i = 0; i < active_table.size(); i++)
                {
                    // Remove y == y_max
                    if (active_table[i].y_max == y)
                        active_table.erase(active_table.begin() + i--);
                }
                std::sort(active_table.begin(), active_table.end(), table_comparison);
                // Drawing
                for (int i = 0; i + 1 < active_table.size(); i += 2)
                {
                    if (active_table[i].x_y_min != active_table[i + 1].x_y_min)
                    {
                        int start = std::ceil(active_table[i].x_y_min);
                        int end;
                        if (active_table[i + 1].x_y_min - std::floor(active_table[i + 1].x_y_min) != 0)
                            end = std::floor(active_table[i + 1].x_y_min);
                        else
                            end = active_table[i + 1].x_y_min - 1;
                        // Colors
                        float ri = (float)(active_table[i].edge->x_y_max_vertex->color.r - active_table[i].edge->x_y_min_vertex->color.r) / (float)(active_table[i].edge->x_y_max_vertex->y - active_table[i].edge->x_y_min_vertex->y);
                        float gi = (float)(active_table[i].edge->x_y_max_vertex->color.g - active_table[i].edge->x_y_min_vertex->color.g) / (float)(active_table[i].edge->x_y_max_vertex->y - active_table[i].edge->x_y_min_vertex->y);
                        float bi = (float)(active_table[i].edge->x_y_max_vertex->color.b - active_table[i].edge->x_y_min_vertex->color.b) / (float)(active_table[i].edge->x_y_max_vertex->y - active_table[i].edge->x_y_min_vertex->y);
                        float rf = (float)(active_table[i + 1].edge->x_y_max_vertex->color.r - active_table[i + 1].edge->x_y_min_vertex->color.r) / (float)(active_table[i + 1].y_max - active_table[i + 1].edge->y_min);
                        float gf = (float)(active_table[i + 1].edge->x_y_max_vertex->color.g - active_table[i + 1].edge->x_y_min_vertex->color.g) / (float)(active_table[i + 1].y_max - active_table[i + 1].edge->y_min);
                        float bf = (float)(active_table[i + 1].edge->x_y_max_vertex->color.b - active_table[i + 1].edge->x_y_min_vertex->color.b) / (float)(active_table[i + 1].y_max - active_table[i + 1].edge->y_min);
                        float redi = (float)active_table[i].edge->x_y_min_vertex->color.r;
                        float greeni = (float)active_table[i].edge->x_y_min_vertex->color.g;
                        float bluei = (float)active_table[i].edge->x_y_min_vertex->color.b;
                        for (int j = 0; j < y - active_table[i].edge->y_min; j++)
                        {
                            redi += ri;
                            greeni += gi;
                            bluei += bi;
                        }
                        SDL_SetRenderDrawColor(renderer, redi, greeni, bluei, 255);
                        SDL_RenderDrawPoint(renderer, start, y);
                        float redf = (float)active_table[i + 1].edge->x_y_min_vertex->color.r;
                        float greenf = (float)active_table[i + 1].edge->x_y_min_vertex->color.g;
                        float bluef = (float)active_table[i + 1].edge->x_y_min_vertex->color.b;
                        for (int j = 0; j < y - active_table[i + 1].edge->y_min; j++)
                        {
                            redf += rf;
                            greenf += gf;
                            bluef += bf;
                        }
                        SDL_SetRenderDrawColor(renderer, redf, greenf, bluef, 255);
                        SDL_RenderDrawPoint(renderer, end, y);
                        float x_delta = end - start;
                        float txr = (redf - redi) / x_delta;
                        float txg = (greenf - greeni) / x_delta;
                        float txb = (bluef - bluei) / x_delta;
                        for (int x = start + 1; x < end; x++)
                        {
                            redi += txr;
                            greeni += txg;
                            bluei += txb;
                            SDL_SetRenderDrawColor(renderer, redi, greeni, bluei, 255);
                            SDL_RenderDrawPoint(renderer, x, y);
                        }
                    }
                    // Incrementing
                    active_table[i].x_y_min = active_table[i].x_y_min + active_table[i].m_inversed;
                    active_table[i + 1].x_y_min = active_table[i + 1].x_y_min + active_table[i + 1].m_inversed;
                }
            }
        }
    }

    void create_texture(SDL_Renderer *renderer)
    {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, x_res, y_res);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
        SDL_RenderDrawRect(renderer, NULL);
        // Draw Geometry
        // Scanline
        scanline_fill();
        // Vertices
        draw_vertices();
        SDL_SetRenderTarget(renderer, NULL);
    }

    bool static compareEdges(const Edge &a, const Edge &b)
    {
        return a.x_y_min < b.x_y_min;
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

    void set_fill(SDL_Color color)
    {
        fill_color = color;
        reload(renderer);
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_RenderCopy(renderer, texture, NULL, &geometry);
    }

    void set_thickness(int thickness)
    {
        this->thickness = thickness;
        for (Vertex *vertex : vertices)
        {
            vertex->thickness = thickness;
        }
        reload(renderer);
    }

    void open_polygon(float x, float y, SDL_Color color = {0, 0, 0, 255})
    {
        if (creating_polygon)
        {
            printf("Não se pode abrir um polígono quando um já está aberto.\n");
            std::exit(1);
        }
        this->creating_polygon = true;
        float sx = (float)x_res / (float)geometry.w;
        float sy = (float)y_res / (float)geometry.h;
        first = new Vertex(sx * (x - (float)geometry.x), sy * (y - (float)geometry.y));
        first->color = color;
        first->thickness = thickness;
        this->vertices.push_back(first);
        current_vertices++;
        reload(renderer);
    }

    void set_geometry(int x, int y, float width, float height)
    {
        set_position(x, y);
        set_dimensions(width, height);
    }

    void set_dimensions(float width, float height)
    {
        geometry.w = width;
        geometry.h = height;
        float width_difference = width - geometry.w;
        float height_difference = height - geometry.h;
        if (texture != NULL)
        {
            for (Vertex *vertex : vertices)
            {
                vertex->x += width_difference;
                vertex->y += height_difference;
            }
        }
    }

    void draw_polygon(float x, float y, SDL_Color color = {0, 0, 0, 255})
    {
        if (!creating_polygon)
        {
            printf("Não há polígonos sendo criados...\n");
            std::exit(1);
        }
        float sx = (float)x_res / (float)geometry.w;
        float sy = (float)y_res / (float)geometry.h;
        Vertex *vertex = new Vertex(sx * (x - (float)geometry.x), sy * (y - (float)geometry.y));
        vertex->thickness = thickness;
        vertex->color = color;
        vertices.push_back(vertex);
        Edge *edge = new Edge(vertices[vertices.size() - 2], vertices[vertices.size() - 1]);
        edges.push_back(edge);
        temp_edges.push_back(edge);
        current_vertices++;
        reload(renderer);
    }

    Polygon *close_polygon()
    {
        if (!creating_polygon)
        {
            printf("Não há polígonos sendo criados...\n");
            std::exit(1);
        }
        Edge *edge = new Edge(vertices[vertices.size() - 1], first);
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
            polygon->add_edge(edge);
        }
        temp_edges.clear();
        polygons.push_back(polygon);
        creating_polygon = false;
        current_vertices = 0;
        reload(renderer);
        return polygon;
    }

    void get_vertex_pos(Vertex *vertex, int *x, int *y)
    {
        float sx = (float)x_res / (float)geometry.w;
        float sy = (float)y_res / (float)geometry.h;
        *x = (float)vertex->x / sx + (float)geometry.x;
        *y = (float)vertex->y / sy + (float)geometry.y;
    }

    void get_edge_pos(Edge *edge, int *x, int *y)
    {
        float sx = (float)x_res / (float)geometry.w;
        float sy = (float)y_res / (float)geometry.h;
        *x = (float)(((float)edge->a->x + (float)edge->b->x) / 2) / sx + (float)geometry.x;
        *y = (float)(((float)edge->a->y + (float)edge->b->y) / 2) / sy + (float)geometry.y;
    }

    void rescale(float width, float height)
    {
        int new_x_res = ceil((width / scaling)) * scaling;
        int new_y_res = ceil((height / scaling)) * scaling;
        if (new_x_res != x_res || new_y_res != y_res)
        {
            float old_sx = (float)x_res / (float)geometry.w;
            float old_sy = (float)y_res / (float)geometry.h;
            x_res = std::max<int>(new_x_res, scaling);
            y_res = std::max<int>(new_y_res, scaling);
            float new_sx = (float)x_res / (float)geometry.w;
            float new_sy = (float)y_res / (float)geometry.h;
            for (Vertex *vertex : vertices)
            {
                float x = (float)vertex->x / old_sx + (float)geometry.x;
                float y = (float)vertex->y / old_sy + (float)geometry.y;
                vertex->x = new_sx * (x - (float)geometry.x);
                vertex->y = new_sy * (y - (float)geometry.y);
            }
            for (Edge *edge : edges)
                edge->update();
            for (Polygon *polygon : polygons)
                polygon->update();
            if (texture != NULL)
            {
                SDL_DestroyTexture(texture);
                create_texture(renderer);
            }
        }
    }

    void delete_polygon(Polygon *polygon)
    {
        for (int i = 0; i < polygons.size(); i++)
        {
            if (polygon == polygons[i])
            {
                for (Edge *edge : polygons[i]->edges)
                {
                    for (int j = 0; j < edges.size(); j++)
                    {
                        if (edge == edges[j])
                        {
                            for (int k = 0; k < vertices.size(); k++)
                            {
                                if (vertices[k] == edges[j]->a || vertices[k] == edges[j]->b)
                                    vertices.erase(vertices.begin() + k--);
                            }
                            edges.erase(edges.begin() + j--);
                            break;
                        }
                    }
                }
                polygons.erase(polygons.begin() + i--);
                break;
            }
        }
        delete polygon;
        reload(renderer);
    }

    void clear()
    {
        if (creating_polygon)
        {
            temp_edges.clear();
            creating_polygon = false;
            current_vertices = 0;
        }
        polygons.clear();
        vertices.clear();
        edges.clear();
        reload(renderer);
    }

    bool static closest_edge_comparison(Edge *a, Edge *b)
    {
        return a->close_val < b->close_val;
    }

    Edge *closest_edge_touched(int x, int y, float e = 0)
    {
        float sx = (float)x_res / (float)geometry.w;
        float sy = (float)y_res / (float)geometry.h;
        x = sx * ((float)x - (float)geometry.x);
        y = sy * ((float)y - (float)geometry.y);
        std::vector<Edge *> touched_edges;
        for (Edge *edge : edges)
        {
            float m = ((float)edge->a->y - (float)edge->b->y) / ((float)edge->a->x - (float)edge->b->x);
            float b = edge->b->y - m * edge->b->x;
            if (x < edge->get_x_min() - (int)e || x > edge->get_x_max() + (int)e || y < edge->y_min - (int)e || y > edge->y_max + (int)e)
                continue;
            float dist = std::abs((float)y - (x * m + b));
            float dist2 = std::abs((float)x - ((float)y - b) / m);
            if (dist <= e && dist2 <= e)
            {
                touched_edges.push_back(edge);
                edge->close_val = dist;
            }
            else if ((float)edge->a->x - (float)edge->b->x == 0)
            {
                if (std::abs(y - edge->y_max) <= e)
                {
                    touched_edges.push_back(edge);
                    edge->close_val = std::abs(y - edge->y_max);
                }
            }
        }
        if (touched_edges.size())
        {
            std::sort(touched_edges.begin(), touched_edges.end(), closest_edge_comparison);
            return touched_edges[0];
        }
        else
            return NULL;
    }

    Vertex *first_vertex_touched(int x, int y, float square_area = 0)
    {
        float sx = (float)x_res / (float)geometry.w;
        float sy = (float)y_res / (float)geometry.h;
        x = sx * ((float)x - (float)geometry.x);
        y = sy * ((float)y - (float)geometry.y);
        if (square_area <= 0)
        {
            for (Vertex *vertex : vertices)
            {
                if (x >= vertex->x - (float)vertex->thickness / 2 && y >= vertex->y - (float)vertex->thickness / 2 && x <= vertex->x + (float)vertex->thickness && y <= vertex->y + (float)vertex->thickness)
                    return vertex;
            }
        }
        else
        {
            for (Vertex *vertex : vertices)
            {
                if (x >= vertex->x - square_area / 2 && y >= vertex->y - square_area / 2 && x <= vertex->x + square_area && y <= vertex->y + square_area)
                    return vertex;
            }
        }
        return NULL;
    }

    void set_colors()
    {
        reload(renderer);
    }
};

#endif