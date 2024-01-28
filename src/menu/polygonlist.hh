#ifndef POLYGONLIST_H
#define POLYGONLIST_H

#include <SDL2/SDL.h>
#include <string>
#include <algorithm>
#include <math.h>
#include "../geometry.hh"
#include "../scene/scene.hh"

class PolygonList : public Component
{
private:
    SDL_Color border_color = {0, 0, 0, 255};
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
        float polygon_rect_w = (float)x_res / 5.00;
        for (int i = 0; i < polygons.size(); i++)
        {
            SDL_Rect polygon_rect{(float)(i % 5) * polygon_rect_w, (float)(i / 5) * polygon_rect_w + 1, polygon_rect_w, polygon_rect_w};
            SDL_RenderDrawRect(renderer, &polygon_rect);
            float polygon_h = polygons[i]->y_max - polygons[i]->y_min;
            float polygon_w = polygons[i]->get_x_max() - polygons[i]->get_x_min();
            float sx = polygon_rect_w/polygon_w;
            float sy = polygon_rect_w/polygon_h;
            Polygon *polygon_copy = new Polygon(polygons[i]);
            float x_min = polygon_copy->get_x_min();
            float y_min = polygon_copy->y_min;
            for (Edge *edge : polygon_copy->edges)
            {
                edge->a->x *= sx;
                edge->b->x *= sx;
                edge->b->y *= sy;
                edge->a->y *= sy;
                edge->update();
            }
            polygon_copy->update();
            float x_diff = polygon_rect.x - polygon_copy->get_x_min();
            float y_diff = polygon_rect.y - polygon_copy->y_min;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            std::vector<SDL_Point> points;
            for (Edge *edge : polygon_copy->edges)
            {
                edge->a->x += x_diff;
                edge->b->x += x_diff;
                edge->b->y += y_diff;
                edge->a->y += y_diff;
                points.push_back({edge->a->x, edge->a->y});
                points.push_back({edge->b->x, edge->b->y});
                SDL_RenderDrawLine(renderer, edge->a->x, edge->a->y, edge->b->x, edge->b->y);
            }
            polygon_copy->update();
            SDL_RenderDrawPoints(renderer, points.data(), points.size());
        }
        SDL_SetRenderTarget(renderer, NULL);
    }

public:
    PolygonList(int width)
    {
        scaling = 8;
        geometry.w = width;
        geometry.h = (float)width / 5;
        rescale();
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_RenderCopy(renderer, texture, NULL, &geometry);
    }

    Polygon *remove_polygon(int x, int y)
    {
        float actual_x = x - geometry.x;
        float actual_y = y - geometry.y;
        int col = actual_x/(int)((float)geometry.w/5.00);
        int row = actual_y/(int)((float)geometry.w/5.00);
        if (col+(row*5) < polygons.size())
        {
            Polygon *val = polygons[col+(row*5)];
            polygons.erase(polygons.begin() + col+(row*5));
            geometry.h = (geometry.w / 5) * (1 + polygons.size() / 5);
            rescale(geometry.w, geometry.h);
            reload(renderer);
            return val;
        }
        else
            return NULL;
    }

    void add_polygon(Polygon *polygon)
    {
        polygons.push_back(polygon);
        geometry.h = (geometry.w / 5) * (1 + polygons.size() / 5);
        rescale(geometry.w, geometry.h);
        reload(renderer);
    }

    void set_dimensions(float width)
    {
        geometry.w = width;
        geometry.h = (geometry.w / 5) * (1 + polygons.size() / 5);
        rescale();
    }

    void clear()
    {
        polygons.clear();
        geometry.h = (geometry.w / 5) * (1 + polygons.size() / 5);
        rescale(geometry.w, geometry.h);
        reload(renderer);
    }
};

#endif