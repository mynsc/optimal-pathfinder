#include "mostar_vertice.hpp"
#include "vertice.hpp"
#include "arista.hpp"
#include <iostream>

void mostrarVertices(ListaVertices verticeOrigen)
{
    std::cout << "\nOrigen: " << verticeOrigen->nombre << std::endl;

    ListaAristas aux = verticeOrigen->AristaAdyacente;

    while (aux != nullptr)
    {
        std::cout << verticeOrigen->nombre;
        std::cout << "  ---" << aux->peso << "---> " << aux->destino->nombre << std::endl;
        
        aux = aux->siguiente;
    }
}