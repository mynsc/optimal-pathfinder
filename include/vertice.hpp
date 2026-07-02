#pragma once

#include <string>

struct Arista;

struct Vertice{
    std::string nombre = "";
    float coordenadasX = 0.0;
    float coordenadasY = 0.0;

    Vertice *siguiente = nullptr;
    Vertice *anterior = nullptr;

    Arista *aristaAdyacente = nullptr;
};

typedef Vertice *vertice;