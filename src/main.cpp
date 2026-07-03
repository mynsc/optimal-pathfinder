#include "grafo.hpp"

#include <iostream>

int main()
{
    vertice cabeza = nullptr;
    vertice v1 = crearVertice("Fac. Ing. Geologica");
    vertice v2 = crearVertice("Fac. Ing. Metalurgica");
    vertice v3 = crearVertice("Fac. Quimica e Ing. Geologica");
    vertice v4 = crearVertice("Fac. Ciencias Matematicas");
    vertice v5 = crearVertice("Fac. Ciencias Biologicas");

    cabeza = v1;
    v1->siguiente = v2;
    v2->siguiente = v3;
    v3->siguiente = v4;
    v4->siguiente = v5;
    v5->siguiente = nullptr;

    enlaceBidireccional(v1, v2, 5, true, 0);
    enlaceBidireccional(v1, v5, 3, true, 0);
    enlaceBidireccional(v1, v3, 4, true, 0);
    enlaceBidireccional(v2, v4, 3, true, 0);
    enlaceBidireccional(v5, v4, 4, true, 0);
    enlaceBidireccional(v3, v4, 1, true, 0);

    int opcion  = 0;
    do
    {
        vertice copia = listaVertices;
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