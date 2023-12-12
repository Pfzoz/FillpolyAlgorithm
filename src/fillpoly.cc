#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "scene.hh"
#include "geometry.hh"
#include "menu.hh"

const int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;
const int VW = SCREEN_WIDTH / 100, VH = SCREEN_HEIGHT / 100;
const SDL_Color SDL_COLOR_WHITE = {255, 255, 255};
const SDL_Color SDL_COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color SDL_COLOR_RED = {255, 0, 0, 255};

Scene *main_scene;
SDL_Window *main_window;

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
    main_window = SDL_CreateWindow("Fillpoly", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    main_scene = new Scene(main_window);
    main_scene->fill(SDL_COLOR_WHITE);
    printf ("Scene Created!\n");
    // U.I
    Button *button = new Button("Jorge", 8*VW, 3*VW);
    button->fill(SDL_COLOR_RED);
    main_scene->add_component(button);
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
        }
        main_scene->render();
    }
    main_scene->destroy();
    delete main_scene;
    return 0;
}