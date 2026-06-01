#include "../includes/MostrarVertice.h++"
#include "../includes/Vertice.h++"

#include <iostream>
#include "../includes/Arista.h++"

void MostrarVertice(ListaVertices verticeOrigen)
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