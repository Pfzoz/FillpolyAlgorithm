#ifndef GEOMETRY
#define GEOMETRY

#include <SDL2/SDL.h>
#include "main-window.hh"
#include <string>

struct Point
{
    std::string label = "";
    int x, y;
    Uint8 r, g, b, a;
};

struct Edge
{
    Point j, k;
    std::string label = "";
};

class Triangle
{
public:
    Point a, b, c;
    Edge m, n, o;
    std::string label;

    Triangle(Point a, Point b, Point c)
    {
        m.j = a;
        m.k = b;
        n.j = b;
        n.k =  c;
        o.j = c;
        o.k = a;
    }
};

#endif