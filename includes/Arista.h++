#pragma once
#include "Vertice.h++"


struct Arista{
    short peso;
    Vertice *origen;

    Vertice *destino;

    Arista *siguiente;
    Arista *anterior;
};
typedef Arista *ListaAristas;