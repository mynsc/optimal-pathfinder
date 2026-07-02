#include "mostar_vertice.hpp"
#include "vertice.hpp"
#include "arista.hpp"
#include <iostream>

void mostrarVertices(vertice origen)
{
    std::cout << "\nOrigen: " << origen->nombre << std::endl;

    arista auxiliar = origen->aristaAdyacente;

    while (auxiliar != nullptr)
    {
        std::cout << origen->nombre;
        std::cout << "  ---(" << auxiliar->peso << ")---> " << auxiliar->destino->nombre << std::endl;
        
        auxiliar = auxiliar->siguiente;
    }
}