#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <algorithm>
#include "scene.hh"
#include "geometry.hh"
#include "menu.hh"
#include "canvas.hh"

// Vars
const int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;
int c_screen_width = SCREEN_WIDTH, c_screen_height = SCREEN_HEIGHT;
const float VW = (float)SCREEN_WIDTH / 100, VH = (float)SCREEN_HEIGHT / 100;
float cvw = VW, cvh = VH;
const SDL_Color SDL_COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color SDL_COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color SDL_COLOR_RED = {255, 0, 0, 255};
const SDL_Color SDL_COLOR_TRANSPARENT = {0, 0, 0, 0};

// Assets
Scene *main_scene;
SDL_Window *main_window;
TTF_Font *default_font;
Button *save_btn, *load_btn;
Label *message_text, *red_text, *green_text, *blue_text;
DialogBox *dialog_red, *dialog_blue, *dialog_green;
Canvas *canvas;

void load_assets()
{
    printf("Loading Assets...");
    default_font = TTF_OpenFont("/home/pedrozoz/Repositories/University/cg/fillpoly/assets/fonts/open-sans/OpenSans-Regular.ttf", 14);
    if (default_font == NULL)
        std::exit(1);
    printf("Assets loaded!\n");
}

void resize(int width, int height)
{
    c_screen_width = width;
    c_screen_height = height;
    cvw = (float)c_screen_width / 100;
    cvh = (float)c_screen_height / 100;
    canvas->set_geometry(30 * cvw, 5 * cvh, 65 * cvw, 90 * cvh);
    // Colors
    float dialogs_area = 18*cvw;
    float dialogs_gap = 1*cvw;
    float dialogs_y = 90*cvh;
    dialog_red->set_geometry(5*cvw, dialogs_y, (dialogs_area)/3, 5*cvh);
    dialog_green->set_geometry(dialog_red->geometry.x+dialog_red->geometry.w+dialogs_gap, dialogs_y, (dialogs_area)/3, 5*cvh);
    dialog_blue->set_geometry(dialog_green->geometry.x+dialog_green->geometry.w+dialogs_gap, dialogs_y, (dialogs_area)/3, 5*cvh);
    red_text->set_position(dialog_red->geometry.x + (dialog_red->geometry.w / 2) - red_text->geometry.w / 2, dialog_red->geometry.y - red_text->geometry.w);
}

void canvas_onclick(int x, int y)
{
    if (canvas->current_vertices == 0)
    {
        canvas->open_polygon(x, y);
    }
    else
        canvas->draw_polygon(x, y);
}

void canvas_onright_click(int x, int y)
{
    if (canvas->current_vertices > 2)
        canvas->close_polygon();
}

Vertex *selected = NULL;
void canvas_onmiddle_click(int x, int y)
{
    Vertex *vertex = canvas->first_vertex_touched(x, y, (c_screen_width + c_screen_height) * 0.012);
    if (vertex != NULL)
    {
        if (selected == NULL)
        {
            selected = vertex;
            selected->color = {255, 0, 0, 255};
        }
        else if (selected == vertex)
        {
            selected->color = {0, 0, 0, 255};
            selected = NULL;
        }
        else
        {
            selected->color = {0, 0, 0, 255};
            selected = vertex;
            selected->color = {255, 0, 0, 255};
        }
    }
}

void handle_color_input(SDL_TextInputEvent event, Component *target)
{
    DialogBox *dialog = (DialogBox*) target;
    if (std::stoi(dialog->get_text_content()) > 255)
    {
        printf("Too high!\n");
        dialog->update_text("255");
    }
}

int main(int argc, char *args[])
{
    /* Resources Init */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Não pode inicializar as funções gráficas do SDL. Encerrando...\n");
        std::exit(1);
    }
    if (TTF_Init() < 0)
    {
        printf("Não pode inicializar as fontes do SDL. Encerrando...\n");
        std::exit(1);
    }
    /* Create Scene */
    printf("Creating scene... ");
    main_window = SDL_CreateWindow("Fillpoly", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    main_scene = new Scene(main_window);
    main_scene->fill(SDL_COLOR_WHITE);
    printf("Scene Created!\n");
    /* Load Assets */
    load_assets();
    /* Create Components */
    canvas = new Canvas(65 * VW, 90 * VH);
    dialog_red = new DialogBox("", 22, default_font, (18*VW)/3, 5*VH);
    dialog_green = new DialogBox("", 22, default_font, (18*VW)/3, 5*VW);
    dialog_blue = new DialogBox("", 22, default_font, (18*VW)/3, 5*VW);
    red_text = new Label("RED", default_font, 22);
    resize(SCREEN_WIDTH, SCREEN_HEIGHT);
    /* Edit Components */
    red_text->set_text_color(SDL_COLOR_RED);
    dialog_red->digits_only = true;
    dialog_green->digits_only = true;
    dialog_blue->digits_only = true;
    dialog_red->set_default_text("0");
    dialog_green->set_default_text("0");
    dialog_blue->set_default_text("0");
    dialog_red->set_ontextinput(handle_color_input);
    dialog_green->set_ontextinput(handle_color_input);
    dialog_blue->set_ontextinput(handle_color_input);
    canvas->set_onclick(canvas_onclick);
    canvas->set_onright_click(canvas_onright_click);
    canvas->set_onmiddle_click(canvas_onmiddle_click);
    /* Add Components */
    main_scene->add_component(canvas);
    main_scene->add_component(dialog_red);
    main_scene->add_component(dialog_green);
    main_scene->add_component(dialog_blue);
    main_scene->add_component(red_text);
    main_scene->onresize(resize);
    /* Main Loop */
    while (!main_scene->quit)
    {
        main_scene->poll();
        main_scene->render();
    }
    main_scene->destroy();
    delete main_scene;
    return 0;
}