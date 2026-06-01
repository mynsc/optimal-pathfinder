#pragma once

#include <string>


struct Arista;
struct Vertice{
    std::string nombre;

    Vertice *siguiente = NULL;
    Vertice *anterior = NULL;

    Arista* AristaAdyacente = NULL;
};

typedef Vertice* ListaVertices;