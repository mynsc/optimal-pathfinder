#include "arista.hpp"
#include "grafo.hpp"
#include "vertice.hpp"
#include "mostar_vertice.hpp"
#include <iostream>

int main()
{
    ListaVertices listaVertices = nullptr;
    ListaVertices v1 = crearVertice("Fac. Ing. Geologica");
    ListaVertices v2 = crearVertice("Fac. Ing. Metalurgica");
    ListaVertices v3 = crearVertice("Fac. Quimica e Ing. Geologica");
    ListaVertices v4 = crearVertice("Fac. Ciencias Matematicas");
    ListaVertices v5 = crearVertice("Fac. Ciencias Biologicas");

    listaVertices = v1;
    v1->siguiente = v2;
    v2->siguiente = v3;
    v3->siguiente = v4;
    v4->siguiente = v5;
    v5->siguiente = nullptr;

    enlaceBidireccional(v1, v2, 5);
    enlaceBidireccional(v1, v5, 3);
    enlaceBidireccional(v1, v3, 4);
    enlaceBidireccional(v2, v4, 3);
    enlaceBidireccional(v5, v4, 4);
    enlaceBidireccional(v3, v4, 1);

    int opcion  = 0;
    do
    {
        ListaVertices copia = listaVertices;
        int i = 1;
        
        std::cout << "\n\tEscoja un vertice para visualizar sus vecinos\t\n\n";
        while (copia != nullptr)
        {
            std::cout << i << ". " << copia->nombre << std::endl;
            copia = copia->siguiente;
            i++;
        }
        std::cout << "0. Salir\n";

        std::cout << "\nIngrese opcion >> ";
        std::cin >> opcion;

        switch (opcion)
        {
        case 1:
            mostrarVertices(v1);
            break;
        case 2:
            mostrarVertices(v2);
            break;
        case 3:
            mostrarVertices(v3);
            break;
        case 4:
            mostrarVertices(v4);
            break;
        case 5:
            mostrarVertices(v5);
            break;
        case 0:
            std::cout << "Saliendo del programa...\n";
            break;
        default:
            std::cout << "Opcion invalida\n";
        }
        
    } while (opcion != 0);

    return 0;
}