#pragma once

#include <string>

struct Arista;

struct Vertice{
    std::string nombre;

    Vertice *siguiente;
    Vertice *anterior;

    Arista* AristaAdyacente;
};

typedef Vertice* ListaVertices;