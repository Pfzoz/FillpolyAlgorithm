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
    int x, y;
    int thickness = 1;
    std::string name;
    SDL_Color color = {0, 0, 0, 255};

    Vertex() {}

    Vertex(const Vertex *other)
    {
        this->x = other->x;
        this->y = other->y;
        this->name = other->name;
        this->color = other->color;
    }

    Vertex(int x, int y, std::string name = "")
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
    Vertex *a, *b, *x_y_min_vertex, *x_y_max_vertex;
    std::string name;
    int y_min, x_y_min, y_max;
    float m_inversed;

    Edge(const Edge *other)
    {
        this->a = new Vertex(other->a);
        this->b = new Vertex(other->b);
        this->name = other->name;
        this->y_min = other->y_min;
        this->x_y_min = other->x_y_min;
        this->y_max = other->y_max;
        this->m_inversed = other->m_inversed;
    }

    float get_x_max()
    {
        return a->x >= b->x ? a->x : b->x;
    }

    float get_x_min()
    {
        return a->x <= b->x ? a->x : b->x;
    }

    void update()
    {
        if (a->y <= b->y)
        {
            this->y_min = a->y;
            this->y_max = b->y;
            this->x_y_min = a->x;
            x_y_min_vertex = a;
            x_y_max_vertex = b;
        }
        else
        {
            this->y_min = b->y;
            this->y_max = a->y;
            this->x_y_min = b->x;
            x_y_min_vertex = b;
            x_y_max_vertex = a;
        }
        m_inversed = ((float)b->x - (float)a->x) / ((float)b->y - (float)a->y);
    }

    ~Edge()
    {
        delete a;
        delete b;
    }

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
            x_y_min_vertex = a;
            x_y_max_vertex = b;
        }
        else
        {
            this->y_min = b->y;
            this->y_max = a->y;
            this->x_y_min = b->x;
            x_y_min_vertex = b;
            x_y_max_vertex = a;
        }
        m_inversed = ((float)b->x - (float)a->x) / ((float)b->y - (float)a->y);
    }
};

class Polygon
{
public:
    std::vector<Edge *> edges;
    std::string name;
    int y_max = 0, y_min = 0;

    ~Polygon()
    {
        for (int i = 0; i < edges.size(); i++)
        {
            delete edges[i];
        }
    }

    Polygon(const Polygon *other)
    {
        for (int i = 0; i < other->edges.size(); i++)
        {
            this->edges.push_back(new Edge(other->edges[i]));
        }
        this->name = other->name;
        this->y_max = other->y_max;
        this->y_min = other->y_min;
    }

    void add_edge(Edge *edge)
    {
        edges.push_back(edge);
        y_max = edges[0]->y_max;
        for (int i = 1; i < edges.size(); i++)
        {
            if (edges[i]->y_max > y_max)
            {
                y_max = edges[i]->y_max;
            }
        }
        y_min = edges[0]->y_min;
        for (int i = 1; i < edges.size(); i++)
        {
            if (edges[i]->y_min < y_min)
            {
                y_min = edges[i]->y_min;
            }
        }
    }

    float get_x_max()
    {
        float x_max = edges[0]->get_x_max();
        for (int i = 1; i < edges.size(); i++)
        {
            float edge_x_max = edges[i]->get_x_max();
            if (edge_x_max > x_max)
            {
                x_max = edges[i]->get_x_max();
            }
        }
        return x_max;
    }

    float get_x_min()
    {
        float x_min = edges[0]->get_x_min();
        for (int i = 1; i < edges.size(); i++)
        {
            float edge_x_min = edges[i]->get_x_min();
            if (edge_x_min < x_min)
            {
                x_min = edges[i]->get_x_min();
            }
        }
        return x_min;
    }

    void update()
    {
        y_max = edges[0]->y_max;
        for (int i = 1; i < edges.size(); i++)
        {
            if (edges[i]->y_max > y_max)
            {
                y_max = edges[i]->y_max;
            }
        }
        y_min = edges[0]->y_min;
        for (int i = 1; i < edges.size(); i++)
        {
            if (edges[i]->y_min < y_min)
            {
                y_min = edges[i]->y_min;
            }
        }
    }

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