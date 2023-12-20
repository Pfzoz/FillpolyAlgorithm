#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <math.h>

int g_vname_serializer = 0;
int g_ename_serializer = 0;
int g_pname_serializer = 0;

class Vertex
{
public:
    float x, y;
    int thickness = 1;
    std::string name;
    SDL_Color color = {0, 0, 0, 255};

    Vertex(){}

    Vertex(float x, float y, std::string name = "")
    {
        if (name == "")
        {
            this->name = "";
            this->name.push_back('a' + g_vname_serializer++);
        }
        this->x = x;
        this->y = y;
    }
};

class Edge
{
public:
    Vertex *a, *b;
    std::string name;
    float y_min, x_y_min, y_max, m_inversed;

    Edge(Vertex *a, Vertex *b, std::string name = "")
    {
        if (name == "")
        {
            this->name = "";
            this->name.push_back('A' + g_vname_serializer++);
        }
        this->a = a;
        this->b = b;
        if (a->y <= b->y)
        {
            this->y_min = a->y;
            this->y_max = b->y;
            this->x_y_min = a->x;
        }
        else
        {
            this->y_min = b->y;
            this->y_max = a->y;
            this->x_y_min = b->x;
        }
        float m = (b->y - a->y)/(b->x - a->x);
        m_inversed = std::pow(m, -1);
    }
};

class Polygon
{
public:
    std::vector<Edge*> edges;
    std::string name;

    Polygon(std::string name = "")
    {
        if (name == "")
        {
            this->name = "";
            this->name.push_back('0' + g_pname_serializer);
        }
    }
};

#endif