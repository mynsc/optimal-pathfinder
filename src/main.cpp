#include "arista.hpp"
#include "vertice.hpp"
#include "mostar_vertice.hpp"
#include <iostream>

int main()
{
    // VERTICES

    ListaVertices v1 = new Vertice();
    v1->id = 1;
    v1->nombre = "A";

    ListaVertices v2 = new Vertice();
    v2->id = 2;
    v2->nombre = "B";

    ListaVertices v3 = new Vertice();
    v3->id = 3;
    v3->nombre = "C";

    ListaVertices v4 = new Vertice();
    v4->id = 4;
    v4->nombre = "D";

    // ARISTAS DE A

    ListaAristas a1 = new Arista();
    a1->peso = 5;
    a1->origen = v1;
    a1->destino = v2;

    ListaAristas a2 = new Arista();
    a2->peso = 3;
    a2->origen = v1;
    a2->destino = v3;

    a1->siguiente = a2;
    a2->anterior = a1;

    a2->siguiente = nullptr;
    a1->anterior = nullptr;

    v1->AristasAdyacentes = a1;

    // ARISTAS DE B

    ListaAristas a3 = new Arista();
    a3->peso = 2;
    a3->origen = v2;
    a3->destino = v4;

    a3->siguiente = nullptr;
    a3->anterior = nullptr;

    v2->AristasAdyacentes = a3;

    // C y D sin conexiones

    v3->AristasAdyacentes = nullptr;
    v4->AristasAdyacentes = nullptr;

    // MOSTRAR

    mostrarVertices(v1);
    mostrarVertices(v2);
    mostrarVertices(v3);
    mostrarVertices(v4);

    return 0;
}
// EJECUTAR PROGRAMA CON .\src\output\programa.exe