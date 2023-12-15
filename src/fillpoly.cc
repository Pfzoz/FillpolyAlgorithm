#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <algorithm>
#include "scene.hh"
#include "geometry.hh"
#include "menu.hh"
#include "canvas.hh"

const int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;
int current_scwidth = SCREEN_WIDTH, current_scheight = SCREEN_HEIGHT;
const float vw = (float)SCREEN_WIDTH / 100, vh = (float)SCREEN_HEIGHT / 100;
float cvw = vw, cvh = vh;
const SDL_Color SDL_COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color SDL_COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color SDL_COLOR_RED = {255, 0, 0, 255};
const SDL_Color SDL_COLOR_TRANSPARENT = {0, 0, 0, 0};

// Assets
Scene *main_scene;
SDL_Window *main_window;
TTF_Font *default_font;
Button *save_btn, *load_btn;
Label *message_text;
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

void set_positions()
{
    canvas->set_geometry(30 * cvw, 5 * cvh, 65 * cvw, 90 * cvh);
    float new_ptsize = (25 / (float)(SCREEN_WIDTH + SCREEN_HEIGHT)) * (float)(current_scwidth + current_scheight);
    load_btn->ptsize = new_ptsize;
    save_btn->ptsize = new_ptsize;
    dialog_red->set_ptsize(new_ptsize);
    dialog_red->set_dimensions(12*cvw, 5*cvh);
    dialog_green->set_ptsize(new_ptsize);
    dialog_green->set_dimensions(12*cvw, 5*cvh);
    dialog_blue->set_ptsize(new_ptsize);
    dialog_blue->set_dimensions(12*cvw, 5*cvh);
    message_text->set_ptsize(new_ptsize);
    load_btn->background_fit(2 * cvw, 1 * cvh);
    save_btn->background_fit(2 * cvw, 1 * cvh);
    float btns_gap = 1.5 * cvw;
    float btns_size = load_btn->geometry.w + save_btn->geometry.w + btns_gap;
    float btns_pos = (30 * cvw) / 2;
    load_btn->set_position(btns_pos - btns_size / 2, 90 * cvh);
    save_btn->set_position(load_btn->geometry.x + load_btn->geometry.w + btns_gap, 90 * cvh);
    dialog_red->set_position(0, 50 * cvh);
    dialog_green->set_position(dialog_red->geometry.x + dialog_red->geometry.w, 50*cvh);
    dialog_blue->set_position(dialog_green->geometry.x + dialog_green->geometry.w, 50*cvh);
    message_text->set_position(30 * cvw + 65 * cvw / 2 - message_text->geometry.w / 2, 95 * cvh);
}

int main(int argc, char *args[])
{
    // Resources Initialization
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
    // Main Object
    printf("Creating scene... ");
    main_window = SDL_CreateWindow("Fillpoly", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    main_scene = new Scene(main_window);
    main_scene->fill(SDL_COLOR_WHITE);
    printf("Scene Created!\n");
    // Loading Assets
    load_assets();
    // U.I
    canvas = new Canvas(65 * vw, 80 * vh);
    save_btn = new Button("Salvar Arquivo", 8 * vw, 3 * vh, default_font);
    load_btn = new Button("Carregar Arquivo", 8 * vw, 3 * vh, default_font);
    message_text = new Label("Clique com o botão direito para começar a criar um polígono!", 8 * vw, 3 * vh, default_font);
    dialog_red = new DialogBox("", 25, default_font, 8 * vw, 3 * vh);
    dialog_blue = new DialogBox("", 25, default_font, 8 * vw, 3 * vh);
    dialog_green = new DialogBox("", 25, default_font, 8 * vw, 3 * vh);
    dialog_red->digits_only = true;
    dialog_green->digits_only = true;
    dialog_blue->digits_only = true;
    load_btn->fill(SDL_COLOR_TRANSPARENT);
    save_btn->fill(SDL_COLOR_TRANSPARENT);
    canvas->thickness = 3;
    set_positions();
    // Creating Components
    main_scene->add_component(canvas);
    main_scene->add_component(load_btn);
    main_scene->add_component(save_btn);
    main_scene->add_component(message_text);
    main_scene->add_component(dialog_red);
    main_scene->add_component(dialog_blue);
    main_scene->add_component(dialog_green);
    // Main Loop
    bool drawing = false;
    bool quit = false;
    int points_drawn = 0;
    SDL_Event event;
    DialogBox *focus_dialog;
    SDL_StopTextInput();
    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    dialog_red->on_click(event.button.x, event.button.y);
                    dialog_green->on_click(event.button.x, event.button.y);
                    dialog_blue->on_click(event.button.x, event.button.y);
                    if (canvas->in_bounds(event.button.x, event.button.y))
                    {
                        if (!drawing)
                        {
                            drawing = true;
                            points_drawn++;
                            SDL_Color color = {std::stoi(dialog_red->get_text_content()), std::stoi(dialog_green->get_text_content()), std::stoi(dialog_blue->get_text_content()), 255};
                            canvas->open_polygon(event.button.x, event.button.y, color);
                            message_text->set_text_content("Clique com o botão esquerdo para fechar o polígono (É necessário 2 arestas)");
                        }
                        else
                        {
                            SDL_Color color = {std::stoi(dialog_red->get_text_content()), std::stoi(dialog_green->get_text_content()), std::stoi(dialog_blue->get_text_content()), 255};
                            canvas->draw_polygon(event.button.x, event.button.y, color);
                            points_drawn++;
                        }
                    }
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    if (drawing)
                    {
                        if (points_drawn > 2)
                        {
                            canvas->close_polygon();
                            points_drawn = 0;
                            drawing = false;
                            message_text->set_text_content("Clique com o botão direito para começar a criar um polígono!");
                        }
                        else
                        {
                            message_text->set_text_content("É necessário mais de duas arestas para criar um polígono!");
                        }
                    }
                }
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    cvw = ((float)event.window.data1) / 100;
                    cvh = ((float)event.window.data2) / 100;
                    current_scwidth = event.window.data1;
                    current_scheight = event.window.data2;
                    set_positions();
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                dialog_red->key_down(event.key.keysym.sym);
                dialog_blue->key_down(event.key.keysym.sym);
                dialog_green->key_down(event.key.keysym.sym);
            }
            else if (event.type == SDL_TEXTINPUT)
            {
                dialog_red->text_input(event.text);
                dialog_green->text_input(event.text);
                dialog_blue->text_input(event.text);
            }
        }
        main_scene->render();
    }
    main_scene->destroy();
    delete main_scene;
    return 0;
}