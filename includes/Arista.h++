#pragma once

struct Vertice;

struct Arista{
    short peso = 0;
    Vertice *origen;
    Vertice *destino;

    Arista *siguiente;
    Arista *anterior;
};
typedef Arista* ListaAristas;