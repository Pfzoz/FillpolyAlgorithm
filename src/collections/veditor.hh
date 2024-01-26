#ifndef VEDITOR_H
#define VEDITOR_H

#include <SDL2/SDL.h>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include "../geometry.hh"
#include "../scene/scene.hh"
#include "../menu/menu.hh"

class VEditor : public ComponentCollection
{
private:
    int ptsize = 22;
    TTF_Font *font;
    bool selected = false;

public:
    bool is_selected = false;
    DialogBox *red, *green, *blue;
    Vertex *vertex;
    SDL_Rect geometry;

    VEditor(){};

    VEditor(int x, int y, int width, int height, TTF_Font *font)
    {
        this->font = font;
        red = new DialogBox("", ptsize, font);
        green = new DialogBox("", ptsize, font);
        blue = new DialogBox("", ptsize, font);
        red->set_default_text("0");
        green->set_default_text("0");
        blue->set_default_text("0");
        red->digits_only = true;
        green->digits_only = true;
        blue->digits_only = true;
        red->z_index = 1;
        green->z_index = 1;
        blue->z_index = 1;
        red->set_visible(false);
        green->set_visible(false);
        blue->set_visible(false);
        update_geometry(x, y, width, height);
        components.push_back(red);
        components.push_back(green);
        components.push_back(blue);
    }


    void update_red()
    {
        vertex->color.r = std::stoi(red->get_text_content());
    }

    void update_green()
    {
        vertex->color.g = std::stoi(green->get_text_content());
    }
    void update_blue()
    {
        vertex->color.b = std::stoi(blue->get_text_content());
    }

    void update_dimensions(int width, int height)
    {
        geometry.w = width;
        geometry.h = height;
        int w = (geometry.w - (geometry.w * 0.01) * 2) / 3;
        red->geometry.w = w;
        green->geometry.w = w;
        blue->geometry.w = w;
        red->geometry.h = geometry.h;
        green->geometry.h = geometry.h;
        blue->geometry.h = geometry.h;
    }

    void update_position(int x, int y)
    {
        geometry.x = x;
        geometry.y = y;
        int gap = 0.01 * geometry.w;
        red->geometry.x = x;
        green->geometry.x = red->geometry.x + red->geometry.w + gap;
        blue->geometry.x = green->geometry.x + green->geometry.w + gap;
        red->geometry.y = geometry.y;
        green->geometry.y = geometry.y;
        blue->geometry.y = geometry.y;
    }

    void update_geometry(int x, int y, int width, int height)
    {
        update_dimensions(width, height);
        update_position(x, y);
    }

    void select_vertex(Vertex *vertex)
    {
        is_selected = true;
        this->vertex = vertex;
        red->update_text(std::to_string(vertex->color.r));
        green->update_text(std::to_string(vertex->color.g));
        blue->update_text(std::to_string(vertex->color.b));
        red->set_visible(true);
        green->set_visible(true);
        blue->set_visible(true);
    }

    void unselect()
    {
        is_selected = false;
        this->vertex = NULL;
        red->set_visible(false);
        green->set_visible(false);
        blue->set_visible(false);
    }
};

#endif