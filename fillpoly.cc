#include <SDL2/SDL.h>
#include <string>
#include <iostream>

const int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;


int main(int argc, char* args[]) 
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        std::cout << "Não pode inicializar as funções gráficas do SDL. Encerrando...";
    } 
    else
    {
        SDL_Window *window = SDL_CreateWindow("Fillpoly", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
        if (window == NULL)
        {
            std::cout << "Falha na incialização da janela principal";

        }
        else
        {
            SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
            SDL_FillRect(screenSurface, NULL, SDL_MapRGBA(screenSurface->format, 255, 255, 255, 255));
            SDL_UpdateWindowSurface(window);
            SDL_Event event;
            bool quit = false;
            while (quit == false)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    else if (event.type == SDL_KEYDOWN)
                    {
                        printf("Pressed a key!\n");
                        SDL_FillRect(screenSurface, NULL, SDL_MapRGBA(screenSurface->format, 255, 255, 255, 255));
                        SDL_UpdateWindowSurface(window);
                    }
                    else if (event.type == SDL_WINDOWEVENT_SIZE_CHANGED || event.type == SDL_WINDOWEVENT_RESIZED)
                    {
                        std::cout << "Resized";
                        // SDL_BlitSurface(screenSurface, NULL, SDL_GetWindowSurface(window), NULL);
                        
                    }
                }
            }
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 0;
        }    
    }
    return 0;
}