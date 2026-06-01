#pragma once

struct Vertice;

struct Arista{
    short peso = 0;
    Vertice *origen = NULL;
    Vertice *destino = NULL;

    Arista *siguiente = NULL;
    Arista *anterior = NULL;
};
typedef Arista* ListaAristas;