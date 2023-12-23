#ifndef CEDITOR_H
#define CEDITOR_H

#include <SDL2/SDL.h>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include "../scene.hh"
#include "../menu/menu.hh"

class CEditor : public ComponentCollection
{
private:
    int ptsize = 22;
    TTF_Font *font;

    static void handle_textinput(SDL_TextInputEvent event, Component *target)
    {
        DialogBox *dialog = (DialogBox *)target;
        if (std::stoi(dialog->get_text_content()) > 255)
            dialog->update_text("255");
    }

public:
    DialogBox *red_editor = NULL, *green_editor = NULL, *blue_editor = NULL;
    Label *red_label = NULL, *green_label = NULL, *blue_label = NULL;
    SDL_Rect geometry;

    CEditor(){};

    void update_position(int x, int y)
    {
        geometry.x = x;
        geometry.y = y;
        red_editor->set_position(geometry.x, geometry.y - geometry.h);
        green_editor->set_position(red_editor->geometry.x + red_editor->geometry.w, geometry.y - geometry.h);
        blue_editor->set_position(green_editor->geometry.x + green_editor->geometry.w, geometry.y - geometry.h);
        red_label->set_position(red_editor->geometry.x + (red_editor->geometry.w / 2) - red_label->geometry.w / 2, red_editor->geometry.y - red_editor->geometry.h);
        green_label->set_position(green_editor->geometry.x + (green_editor->geometry.w / 2) - green_label->geometry.w / 2, green_editor->geometry.y - green_editor->geometry.h);
        blue_label->set_position(blue_editor->geometry.x + (blue_editor->geometry.w / 2) - red_label->geometry.w / 2, blue_editor->geometry.y - blue_editor->geometry.h);
    }

    void update_dimensions(int width, int height)
    {
        geometry.w = width;
        geometry.h = height;
        for (Component *component : components)
        {
            component->set_dimensions(width / 3, height / 2);
            component->rescale();
        }
    }

    void update_geometry(int x, int y, int width, int height)
    {
        update_dimensions(width, height);
        update_position(x, y);
    }

    void set_ptsize(int ptsize)
    {
        red_label->set_ptsize(ptsize);
        green_label->set_ptsize(ptsize);
        blue_label->set_ptsize(ptsize);
        red_editor->set_ptsize(ptsize);
        green_editor->set_ptsize(ptsize);
        blue_editor->set_ptsize(ptsize);
    }

    CEditor(int x, int y, int width, int height, TTF_Font *font)
    {
        geometry.x = x;
        geometry.y = y;
        geometry.w = width;
        geometry.h = height;
        red_editor = new DialogBox("", ptsize, font);
        green_editor = new DialogBox("", ptsize, font);
        blue_editor = new DialogBox("", ptsize, font);
        red_label = new Label("RED", font, ptsize);
        green_label = new Label("GREEN", font, ptsize);
        blue_label = new Label("BLUE", font, ptsize);
        red_editor->digits_only = true;
        green_editor->digits_only = true;
        blue_editor->digits_only = true;
        red_editor->set_default_text("0");
        green_editor->set_default_text("0");
        blue_editor->set_default_text("0");
        red_editor->set_ontextinput(handle_textinput);
        green_editor->set_ontextinput(handle_textinput);
        blue_editor->set_ontextinput(handle_textinput);
        update_geometry(x, y, width, height);
        components.push_back(red_editor);
        components.push_back(green_editor);
        components.push_back(blue_editor);
        components.push_back(red_label);
        components.push_back(green_label);
        components.push_back(blue_label);
    }
};

#endif