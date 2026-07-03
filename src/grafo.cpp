#include "../include/grafo.hpp"
#include <iostream>

#include <limits>


vertice crearVertice(const std::string &nombre)
{
    vertice v = new Vertice();

    v->nombre = nombre;
    v->siguiente = nullptr;
    v->anterior = nullptr;
    v->aristaAdyacente = nullptr;

    return v;
}

void agregarVertice(vertice &cabeza, vertice v)
{
    if (!cabeza) { cabeza = v; return; }

    vertice actual = cabeza;

    while (actual->siguiente) actual = actual->siguiente;

    actual->siguiente = v;
    v->siguiente = nullptr;
    v->anterior = actual;
}

vertice buscarVertice(vertice cabeza, const std::string &nombre)
{
    vertice actual = cabeza;

    while (actual)
    {
        if (actual->nombre == nombre) return actual;
        actual = actual->siguiente;
    }

    return nullptr;
}

arista crearArista(vertice origen, vertice destino, short peso)
{
    arista a = new Arista();

    a->peso = peso;
    a->origen = origen;
    a->destino = destino;
    a->siguiente = nullptr;
    a->anterior = nullptr;

    return a;
}

void agregarArista(vertice origen, arista arista)
{
    if (!origen) return;

    arista->siguiente = origen->aristaAdyacente;

    if (origen->aristaAdyacente)
        origen->aristaAdyacente->anterior = arista;
    origen->aristaAdyacente = arista;
}

void enlaceBidireccional(vertice a, vertice b, short peso)
{
    if (!a || !b) return;

    arista ab = crearArista(a, b, peso);
    arista ba = crearArista(b, a, peso);

    agregarArista(a, ab);
    agregarArista(b, ba);
}

void liberarGrafo(vertice &cabeza)
{
    vertice actualV = cabeza;

    while (actualV) 
    {
        arista actualA = actualV->aristaAdyacente;

        while (actualA)
        {
            arista siguienteA = actualA->siguiente;
            delete actualA;
            actualA = siguienteA;
        }

        vertice siguienteV = actualV->siguiente;
        delete actualV;
        actualV = siguienteV;
    }

    cabeza = nullptr;
}