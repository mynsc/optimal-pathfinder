#pragma once

struct Vertice;

struct Arista
{
    short peso = 0;
    bool esAccesible = true;
    short nivelRestriccion = 0;
    
    Vertice *origen = nullptr;
    Vertice *destino = nullptr;

    Arista *siguiente = nullptr;
    Arista *anterior = nullptr;
};

typedef Arista *arista;