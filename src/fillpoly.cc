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
DialogBox *vertex_red, *vertex_green, *vertex_blue;
Canvas *canvas;

Vertex *selected = NULL;

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
    message_text->set_position(canvas->geometry.x + (canvas->geometry.w / 2) - message_text->geometry.w / 2, 95 * cvh);
    // Colors
    float dialogs_area = 18 * cvw;
    float dialogs_gap = 1 * cvw;
    float dialogs_y = 90 * cvh;
    dialog_red->set_geometry(5 * cvw, dialogs_y, (dialogs_area) / 3, 5 * cvh);
    dialog_green->set_geometry(dialog_red->geometry.x + dialog_red->geometry.w + dialogs_gap, dialogs_y, (dialogs_area) / 3, 5 * cvh);
    dialog_blue->set_geometry(dialog_green->geometry.x + dialog_green->geometry.w + dialogs_gap, dialogs_y, (dialogs_area) / 3, 5 * cvh);
    red_text->set_position(dialog_red->geometry.x + (dialog_red->geometry.w / 2) - red_text->geometry.w / 2, dialog_red->geometry.y - red_text->geometry.w);
    // Vertex
    vertex_red->set_dimensions(dialogs_area / 3, 5 * cvh);
}

void canvas_onclick(int x, int y, bool hit)
{
    if (hit)
    {
        SDL_Color color = {std::stoi(dialog_red->get_text_content()), std::stoi(dialog_green->get_text_content()), std::stoi(dialog_blue->get_text_content()), 255};
        if (canvas->current_vertices == 0)
        {
            message_text->update_text("Clique com o botão direito para criar o polígono. (É necessário 3+ vértices)");
            canvas->open_polygon(x, y, color);
        }
        else
            canvas->draw_polygon(x, y, color);
    }
}

void canvas_onright_click(int x, int y, bool hit)
{
    if (canvas->current_vertices > 2 && hit)
    {
        message_text->update_text("Clique com o botão esquerdo para começar a desenhar um polígono.");
        canvas->close_polygon();
    }
}

void canvas_onmiddle_click(int x, int y, bool hit)
{
    if (hit)
    {
        selected = canvas->first_vertex_touched(x, y, (c_screen_width + c_screen_height) * 0.012);
        if (selected != NULL)
        {
            int screen_x, screen_y;
            canvas->get_vertex_pos(selected, &screen_x, &screen_y);
            vertex_red->set_position(screen_x - vertex_red->geometry.w / 2, screen_y - selected->thickness - vertex_red->geometry.h);
            vertex_red->update_text(std::to_string(selected->color.r));
            vertex_red->set_visible(true);
        }
    }
}

void handle_miss_click(int x, int y, bool hit)
{
    if (!hit && selected != NULL)
    {
        selected = NULL;
        vertex_red->set_visible(false);
    }
}

void handle_color_input(SDL_TextInputEvent event, Component *target)
{
    DialogBox *dialog = (DialogBox *)target;
    if (std::stoi(dialog->get_text_content()) > 255)
        dialog->update_text("255");
}

void handle_vertex_color_input(SDL_TextInputEvent event, Component *target)
{
    DialogBox *dialog = (DialogBox *)target;
    if (std::stoi(dialog->get_text_content()) > 255)
    {
        dialog->update_text("255");
    }
    if (dialog == vertex_red)
    {
        SDL_Color color = {std::stoi(dialog->get_text_content()),
                           selected->color.g,
                           selected->color.b,
                           selected->color.a};
        selected->color = color;
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
    message_text = new Label("Clique com o botão esquerdo para começar a desenhar um polígono.", default_font, 22);
    dialog_red = new DialogBox("", 22, default_font, (18 * VW) / 3, 5 * VH);
    dialog_green = new DialogBox("", 22, default_font, (18 * VW) / 3, 5 * VW);
    dialog_blue = new DialogBox("", 22, default_font, (18 * VW) / 3, 5 * VW);
    vertex_red = new DialogBox("", 22, default_font, (18 * VW) / 3, 5 * VW);
    red_text = new Label("RED", default_font, 22);
    resize(SCREEN_WIDTH, SCREEN_HEIGHT);
    /* Edit Components */
    red_text->set_text_color(SDL_COLOR_RED);
    vertex_red->set_visible(false);
    vertex_red->z_index = 1;
    vertex_red->set_default_text("0");
    vertex_red->set_ontextinput(handle_vertex_color_input);
    vertex_red->set_onclick(handle_miss_click);
    // Colors
    dialog_red->digits_only = true;
    dialog_green->digits_only = true;
    dialog_blue->digits_only = true;
    dialog_red->set_default_text("0");
    dialog_green->set_default_text("0");
    dialog_blue->set_default_text("0");
    dialog_red->set_ontextinput(handle_color_input);
    dialog_green->set_ontextinput(handle_color_input);
    dialog_blue->set_ontextinput(handle_color_input);
    // Canvas
    canvas->set_onclick(canvas_onclick);
    canvas->set_onright_click(canvas_onright_click);
    canvas->set_onmiddle_click(canvas_onmiddle_click);
    /* Add Components */
    main_scene->add_component(canvas);
    main_scene->add_component(message_text);
    main_scene->add_component(dialog_red);
    main_scene->add_component(dialog_green);
    main_scene->add_component(dialog_blue);
    main_scene->add_component(red_text);
    main_scene->add_component(vertex_red);
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