#include "arista.hpp"
#include "grafo.hpp"
#include "vertice.hpp"
#include "mostar_vertice.hpp"
#include <iostream>

int main()
{
    // vertices

    ListaVertices listaVertices = nullptr;
    ListaVertices v1 = crearVertice("Fac. Ing. Geologica");
    ListaVertices v2 = crearVertice("Fac. Ing. Metalurgica");
    ListaVertices v3 = crearVertice("Fac. Quimica e Ing. Geologica");
    ListaVertices v4 = crearVertice("Fac. Ciencias Matematicas");
    ListaVertices v5 = crearVertice("Fac. Ciencias Biologicas");

    // circularidad

    listaVertices = v1;
    v1->siguiente = v2;
    v2->siguiente = v3;
    v3->siguiente = v4;
    v4->siguiente = v5;
    v5->siguiente = listaVertices;

    // enlazar bidireccionalmente aristas

    enlaceBidireccional(v1, v2, 5);
    enlaceBidireccional(v1, v5, 3);
    enlaceBidireccional(v1, v3, 4);
    enlaceBidireccional(v2, v4, 3);
    enlaceBidireccional(v5, v4, 4);
    enlaceBidireccional(v3, v4, 1);

    // ver rutas y pesos de cada vertice en especifico

    ListaVertices copia = listaVertices;
    int opcion  = 0;
    do
    {
        //system("cls");

        std::cout << "\tINGRESE VERTICE A VER SUS VECINOS:\t\n\n";
        std::cout << "presionar 0 para salir\n " << std::endl;
        int i = 1;
        do
        {
            std::cout << "OPCION " << i << ": " << copia->nombre << std::endl;
            copia = copia->siguiente;
            i++;

        } while (copia != listaVertices);
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
            std::cout << "Saliendo del programa";
            break;
        default:
            std::cout << "Opcion invalida\n";
        }
        //system("pause");
    } while (opcion != 0);

    return 0;
}