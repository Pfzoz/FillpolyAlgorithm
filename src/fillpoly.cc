#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "scene.hh"
#include "geometry.hh"
#include "menu.hh"
#include "canvas.hh"

const int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;
int current_scwidth = SCREEN_WIDTH, current_scheight = SCREEN_HEIGHT;
const float vw = (float) SCREEN_WIDTH / 100, vh = (float) SCREEN_HEIGHT / 100;
int cvw = vw, cvh = vh;
const SDL_Color SDL_COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color SDL_COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color SDL_COLOR_RED = {255, 0, 0, 255};
const SDL_Color SDL_COLOR_TRANSPARENT = {0, 0, 0, 0};

// Assets
Scene *main_scene;
SDL_Window *main_window;
TTF_Font *default_font;
Button *save_btn, *load_btn;
Canvas *canvas;

void load_assets()
{
    printf("Loading Assets...");
    default_font = TTF_OpenFont("/home/pedrozoz/Repositories/University/cg/fillpoly/assets/fonts/open-sans/OpenSans-Regular.ttf", 14);
    if (default_font == NULL) std::exit(1);
    printf("Assets loaded!\n");
}

void set_positions()
{
    canvas->x = 30*cvw;
    canvas->y = 5*cvh;
    load_btn->y = 95*cvh;
    load_btn->x = 2*cvw;
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
    printf ("Scene Created!\n");
    // Loading Assets
    load_assets();
    // U.I
    canvas = new Canvas(65*vw, 65*vh);
    load_btn = new Button("Carregar Arquivo", 8*vw, 3*vh, default_font);
    load_btn->background_fit();
    load_btn->fill(SDL_COLOR_TRANSPARENT);
    set_positions();
    // Creating Components
    main_scene->add_component(canvas);
    main_scene->add_component(load_btn);
    // Main Loop
    bool quit = false;
    SDL_Event event;
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
                    if (load_btn->in_bounds(event.button.x, event.button.y))
                    {
                        load_btn->set_dimensions(load_btn->get_width() + 1, load_btn->get_height());
                    }
                }
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_MAXIMIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    cvw = ((float) event.window.data1) / 100;
                    cvh = ((float) event.window.data2) / 100;
                    float new_ptratio = ((float) event.window.data1) / ((float) SCREEN_WIDTH);
                    TTF_SetFontSize(default_font, 18*new_ptratio);
                    load_btn->background_fit();
                    set_positions();
                }
            }
        }
        main_scene->render();
    }
    main_scene->destroy();
    delete main_scene;
    return 0;
}