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
DialogBox *dialog;
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
    dialog->set_ptsize(new_ptsize);
    dialog->set_dimensions(12*cvw, 5*cvh);
    message_text->set_ptsize(new_ptsize);
    load_btn->background_fit(2 * cvw, 1 * cvh);
    save_btn->background_fit(2 * cvw, 1 * cvh);
    float btns_gap = 1.5 * cvw;
    float btns_size = load_btn->geometry.w + save_btn->geometry.w + btns_gap;
    float btns_pos = (30 * cvw) / 2;
    load_btn->set_position(btns_pos - btns_size / 2, 90 * cvh);
    save_btn->set_position(load_btn->geometry.x + load_btn->geometry.w + btns_gap, 90 * cvh);
    dialog->set_position(0, 50 * cvh);
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
    message_text = new Label("Clique para começar a criar um polígono!", 8 * vw, 3 * vh, default_font);
    dialog = new DialogBox("", 25, default_font, 8 * vw, 3 * vh);
    load_btn->fill(SDL_COLOR_TRANSPARENT);
    save_btn->fill(SDL_COLOR_TRANSPARENT);
    set_positions();
    // Creating Components
    main_scene->add_component(canvas);
    main_scene->add_component(load_btn);
    main_scene->add_component(save_btn);
    main_scene->add_component(message_text);
    main_scene->add_component(dialog);
    // Main Loop
    bool quit = false;
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
                    if (dialog->in_bounds(event.button.x, event.button.y) && !SDL_IsTextInputActive())
                    {
                        SDL_StartTextInput();
                        focus_dialog = dialog;
                    }
                    else if (!dialog->in_bounds(event.button.x, event.button.y) && SDL_IsTextInputActive())
                    {
                        SDL_StopTextInput();
                        focus_dialog = NULL;
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
                if (SDL_IsTextInputActive())
                {
                    if (event.key.keysym.sym == SDLK_BACKSPACE && focus_dialog->get_text_content().length() > 0)
                    {
                        std::string new_text = focus_dialog->get_text_content();
                        new_text.pop_back();
                        focus_dialog->update(new_text);
                    }
                }
            }
            else if (event.type == SDL_TEXTINPUT)
            {
                std::string new_text = focus_dialog->get_text_content();
                new_text.push_back(event.text.text[0]);
                focus_dialog->update(new_text);
            }
        }
        main_scene->render();
    }
    main_scene->destroy();
    delete main_scene;
    return 0;
}