#include "../include/mostar_vertice.hpp"
#include "../include/vertice.hpp"
#include "../include/arista.hpp"
#include <iostream>

void mostrarVertices(ListaVertices vertice)
{
    std::cout << "\nVertice: " << vertice->nombre << std::endl;

    ListaAristas aux = vertice->AristasAdyacentes;

    while (aux != nullptr)
    {
        std::cout << "  --> "
                  << aux->destino->nombre
                  << " (peso: "
                  << aux->peso
                  << ")" << std::endl;

        aux = aux->siguiente;
    }
}