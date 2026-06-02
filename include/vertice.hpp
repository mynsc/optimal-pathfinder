#pragma once

#include <string>

struct Arista;

struct Vertice{
    std::string nombre = "";

    Vertice *siguiente = nullptr;
    Vertice *anterior = nullptr;

    Arista* AristaAdyacente = nullptr;
};

typedef Vertice* ListaVertices;