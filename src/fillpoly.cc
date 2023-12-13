#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
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
    load_btn->set_position(2*cvw, 90*cvh);
    load_btn->background_fit();
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
    load_btn = new Button("Carregar Arquivo", 8 * vw, 3 * vh, default_font);
    load_btn->fill(SDL_COLOR_TRANSPARENT);
    TTF_SetFontSize(default_font, 22);
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
        }
        main_scene->render();
    }
    main_scene->destroy();
    delete main_scene;
    return 0;
}