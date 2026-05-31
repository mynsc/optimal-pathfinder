#include "MostarVertice.h++"
#include "Vertice.h++"

#include <iostream>
#include <Arista.h++>

void mostrarVertices(ListaVertices vertice)
{
    std::cout << "\nOrigen: " <<std::endl;
    
    ListaAristas aux = vertice->AristasAdyacentes;
    
    while (aux != nullptr)
    {
        std::cout<<vertice->nombre << "  --"<<aux->peso<<"-> "<< aux->destino->nombre;
        aux = aux->siguiente;
    }
}   