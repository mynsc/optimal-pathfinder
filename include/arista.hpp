#pragma once

struct Vertice;

struct Arista{
    short peso;
    Vertice *origen;
    Vertice *destino;

    Arista *siguiente;
    Arista *anterior;
};
typedef Arista* ListaAristas;