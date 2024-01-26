#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include <SDL2/SDL.h>
#include <string>
#include <algorithm>
#include <math.h>
#include "../scene/scene.hh"

class ColorWheel : public Component
{
private:
    float luminance = 0.5;
    SDL_Color background_color = {0, 0, 0, 0};
    SDL_Color border_color = {0, 0, 0, 255};
    SDL_Color outer_border_color = {0, 0, 0, 0};

    void hsvToRgb(float hue, float saturation, float value, int *red, int *green, int *blue)
    {
        int hi = static_cast<int>(std::floor(hue / 60.0)) % 6;
        float f = hue / 60.0 - std::floor(hue / 60.0);

        float p = value * (1 - saturation);
        float q = value * (1 - f * saturation);
        float t = value * (1 - (1 - f) * saturation);

        switch (hi)
        {
        case 0:
            *red = static_cast<int>(value * 255);
            *green = static_cast<int>(t * 255);
            *blue = static_cast<int>(p * 255);
            break;
        case 1:
            *red = static_cast<int>(q * 255);
            *green = static_cast<int>(value * 255);
            *blue = static_cast<int>(p * 255);
            break;
        case 2:
            *red = static_cast<int>(p * 255);
            *green = static_cast<int>(value * 255);
            *blue = static_cast<int>(t * 255);
            break;
        case 3:
            *red = static_cast<int>(p * 255);
            *green = static_cast<int>(q * 255);
            *blue = static_cast<int>(value * 255);
            break;
        case 4:
            *red = static_cast<int>(t * 255);
            *green = static_cast<int>(p * 255);
            *blue = static_cast<int>(value * 255);
            break;
        case 5:
            *red = static_cast<int>(value * 255);
            *green = static_cast<int>(p * 255);
            *blue = static_cast<int>(q * 255);
            break;
        }
    }

    void create_texture(SDL_Renderer *renderer)
    {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, x_res, y_res);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
        SDL_RenderClear(renderer);
        for (int i = -radius; i < radius; i++)
        {
            for (int j = -radius; j < radius; j++)
            {
                float distance = std::sqrt(i * i + j * j);
                if (distance <= radius)
                {
                    int phi = ((std::atan2(j, i) + M_PI)/(2*M_PI))*360;
                    int r, g, b;
                    hsvToRgb(phi, distance / radius, luminance, &r, &g, &b);
                    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                    SDL_RenderDrawPoint(renderer, i + x_res / 2, j + y_res / 2);
                }
            }
        }
        std::vector<SDL_Point> points;
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
        for (float i = 0; i < 360; i += 0.1)
        {
            int x = radius * std::cos(i) + x_res / 2;
            int y = radius * std::sin(i) + y_res / 2;
            points.push_back({x, y});
        }
        SDL_RenderDrawPoints(renderer, points.data(), points.size());
        SDL_SetRenderTarget(renderer, NULL);
    }

public:
    float radius;

    ColorWheel(int x, int y, float radius)
    {
        scaling = 8;
        geometry.x = x;
        geometry.y = y;
        this->radius = radius;
        geometry.w = radius * 2;
        geometry.h = radius * 2;
        rescale();
    }

    void get_color(int x, int y, int *red, int *green, int *blue)
    {
        float actual_x = x-(geometry.x+geometry.w/2);
        float actual_y = y-(geometry.y+geometry.h/2);
        float distance = std::sqrt(actual_x*actual_x+actual_y*actual_y);
        if (distance < radius)
        {
            int phi = ((std::atan2(actual_y, actual_x) + M_PI)/(2*M_PI))*360;
            hsvToRgb(phi, distance / radius, luminance, red, green, blue);
        }
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_RenderCopy(renderer, texture, NULL, &geometry);
    }

    void set_radius(float radius)
    {
        this->radius = radius;
        set_dimensions(radius * 2, radius * 2);
    }

    void fill(SDL_Color color)
    {
        this->background_color = color;
        if (texture != NULL)
        {
            SDL_DestroyTexture(texture);
            create_texture(renderer);
        }
    }

    void set_luminance(float luminance)
    {
        this->luminance = luminance;
        reload(renderer);
    }

    bool in_bounds(int x, int y)
    {
        return x >= geometry.x && y >= geometry.y && x <= (geometry.x + geometry.w) && y <= (geometry.y + geometry.h) ? true : false;
    }
};

#endif