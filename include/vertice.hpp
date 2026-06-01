#pragma once

#include <string>

struct Arista;

struct Vertice{
    int id;
    std::string nombre;

    Arista* AristasAdyacentes;
};

typedef Vertice* ListaVertices;