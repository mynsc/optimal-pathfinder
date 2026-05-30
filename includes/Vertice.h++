#pragma once
#include "Arista.h++"

#include <string>
struct Vertice{
    int id;
    std::string nombre;
    ListaAristas AristasAdyacentes;
};

