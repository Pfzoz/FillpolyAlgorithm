#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "main-window.hh"
#include "geometry.hh"

const int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;
const SDL_Color WHITE = {255, 255, 255};
const SDL_Color BLACK = {0, 0, 0};

TTF_Font *text_font;
Component *poligon_btn, *poligon_btn_dark, *poligon_btn_red, *warning_text;
MainWindow *main_window;

void load_assets()
{
    text_font = TTF_OpenFont("assets/fonts/open-sans/OpenSans-Regular.ttf", 14);
    if (text_font == NULL)
    {
        printf("Falha ao carregar a fonte: assets/fonts/open-sans/OpenSans-Regular.ttf\n");
        std::exit(1);
    }
    poligon_btn = new Component(main_window->sdl_renderer, "assets/poligon-btn/bright.png");
    poligon_btn_dark = new Component(main_window->sdl_renderer, "assets/poligon-btn/dark.png");
    poligon_btn_red = new Component(main_window->sdl_renderer, "assets/poligon-btn/red.png");
    warning_text = new Component(main_window->sdl_renderer, TTF_RenderText_Solid(text_font, "Algoritmo Fillpoly!", BLACK));
}

int main(int argc, char *args[])
{
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
    printf("Creating scene...\n");
    main_window = new MainWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
    main_window->setFillColor(255, 255, 255, 255);
    printf("Scene created.\n");
    // Loading stuff;
    printf("Loading assets...\n");
    load_assets();
    printf("Assets loaded.\n");
    // Setting Stuff
    printf("Preparing scene...\n");
    warning_text->x = (SCREEN_WIDTH / 2) - (warning_text->width / 2);
    warning_text->y = SCREEN_HEIGHT * 0.90;
    main_window->draw_component(poligon_btn);
    main_window->draw_component(poligon_btn_dark);
    main_window->draw_component(poligon_btn_red);
    main_window->draw_component(warning_text);
    poligon_btn_dark->hidden = true;
    poligon_btn_red->hidden = true;
    printf("Scene loaded.\n");
    // Triangle stuff
    bool drawing_triangle = false;
    int points_counter = 0;
    Point triangle_points[3];
    //
    int screen_width = SCREEN_WIDTH, screen_height = SCREEN_HEIGHT;
    SDL_Event event;
    bool quit = false;

    while (quit == false)
    {
        main_window->render();

        // Main Loop
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    SDL_GetWindowSize(main_window->sdl_window, &screen_width, &screen_height);
                    warning_text->x = (screen_width / 2) - (warning_text->width / 2);
                    main_window->render();
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                if (poligon_btn->in_bounds(mouse_x, mouse_y) && !drawing_triangle)
                {
                    drawing_triangle = true;
                    poligon_btn_dark->hidden = true;
                    poligon_btn->hidden = true;
                    poligon_btn_red->hidden = false;
                    warning_text->setTexture(main_window->sdl_renderer, TTF_RenderText_Solid(text_font, "Insira o primeiro ponto do polígono.", BLACK));
                    warning_text->x = (screen_width / 2) - (warning_text->width / 2);
                    main_window->render();
                }
                else if (drawing_triangle)
                {
                    triangle_points[points_counter].x = mouse_x;
                    triangle_points[points_counter].y = mouse_y;
                    triangle_points[points_counter].r = 0;
                    triangle_points[points_counter].g = 0;
                    triangle_points[points_counter].b = 0;
                    triangle_points[points_counter].a = 255;
                    triangle_points[points_counter].label = "JORGE";
                    main_window->draw_point(mouse_x, mouse_y, 0, 0, 0, 255, "JORGE");
                    if (points_counter == 0)
                    {
                        warning_text->setTexture(main_window->sdl_renderer, TTF_RenderText_Solid(text_font, "Insira o segundo ponto do polígono.", BLACK));
                    }
                    else if (points_counter == 1)
                    {
                        warning_text->setTexture(main_window->sdl_renderer, TTF_RenderText_Solid(text_font, "Insira o terceiro ponto do polígono.", BLACK));
                    }
                    if (++points_counter == 3)
                    {
                        main_window->draw_triangle(triangle_points[0], triangle_points[1], triangle_points[2], "JORGE");
                        points_counter = 0;
                        drawing_triangle = false;
                        poligon_btn_red->hidden = true;
                        poligon_btn->hidden = false;
                        poligon_btn_dark->hidden = true;
                        warning_text->setTexture(main_window->sdl_renderer, TTF_RenderText_Solid(text_font, "Polígono construído.", BLACK));
                    }
                    warning_text->x = (screen_width / 2) - (warning_text->width / 2);
                    main_window->render();
                }
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                if (!drawing_triangle)
                {
                    if (poligon_btn->in_bounds(mouse_x, mouse_y) && !poligon_btn->hidden)
                    {
                        poligon_btn->hidden = true;
                        poligon_btn_dark->hidden = false;
                        main_window->render();
                    }
                    else if (!poligon_btn->in_bounds(mouse_x, mouse_y) && poligon_btn->hidden)
                    {
                        poligon_btn->hidden = false;
                        poligon_btn_dark->hidden = true;
                        main_window->render();
                    }
                }
            }
        }
    }
    main_window->destroy();
    delete main_window;
    SDL_Quit();
    return 0;
}