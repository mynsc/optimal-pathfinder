#include <iostream>
#include "../includes/Arista.hpp"
#include "../includes/Vertice.hpp"
#include "../includes/MostrarVertice.hpp"
int main()
{
    // VERTICES

    ListaVertices listaVertices = NULL;

    // ARISTAS

    ListaVertices v1 = new Vertice();
    v1->nombre = "Fac. Ing. Geologica";

    ListaVertices v2 = new Vertice();
    v2->nombre = "Fac. Ing. Metalurgica";

    ListaVertices v3 = new Vertice();
    v3->nombre = "Fac. Quimica e Ing. Geologica";

    ListaVertices v4 = new Vertice();
    v4->nombre = "Fac. Ciencias Matematicas";

    ListaVertices v5 = new Vertice();
    v5->nombre = "Fac. Ciencias Biologicas";

    listaVertices = v1;
    v1->siguiente = v2;
    v2->siguiente = v3;
    v3->siguiente = v4;
    v4->siguiente = v5;
    v5->siguiente = listaVertices; // CIRCULARIDAD DE LA LISTA DE VERTICES

    // a -> ARISTA , ap -> ARISTA INVERSA  (CREACION DE ARISTAS BIDIRECCIONALES) FACILITA LA NAVEGACION EN AMBOS SENTIDOS
    ListaAristas a1 = new Arista();
    a1->peso = 5;
    a1->origen = v1;
    a1->destino = v2;

    ListaAristas a1p = new Arista();
    a1p->peso = 5;
    a1p->origen = v2;
    a1p->destino = v1;

    ListaAristas a2 = new Arista();
    a2->peso = 3;
    a2->origen = v1;
    a2->destino = v5;

    ListaAristas a2p = new Arista();
    a2p->peso = 3;
    a2p->origen = v5;
    a2p->destino = v1;

    ListaAristas a3 = new Arista();
    a3->peso = 4;
    a3->origen = v1;
    a3->destino = v3;

    ListaAristas a3p = new Arista();
    a3p->peso = 4;
    a3p->origen = v3;
    a3p->destino = v1;

    ListaAristas a4 = new Arista();
    a4->peso = 3;
    a4->origen = v2;
    a4->destino = v4;

    ListaAristas a4p = new Arista();
    a4p->peso = 3;
    a4p->origen = v4;
    a4p->destino = v2;

    ListaAristas a5 = new Arista();
    a5->peso = 4;
    a5->origen = v5;
    a5->destino = v4;

    ListaAristas a5p = new Arista();
    a5p->peso = 4;
    a5p->origen = v4;
    a5p->destino = v5;

    ListaAristas a6 = new Arista();
    a6->peso = 1;
    a6->origen = v3;
    a6->destino = v4;

    ListaAristas a6p = new Arista();
    a6p->peso = 1;
    a6p->origen = v4;
    a6p->destino = v3;

    // INTEGRACION DE LAS ARISTAS A LOS VERTICES (LISTA DE ADYACENCIA)
    v1->AristaAdyacente = a1;
    a1->siguiente = a2;
    a2->siguiente = a3;

    v2->AristaAdyacente = a1p;
    a1p->siguiente = a4;

    v3->AristaAdyacente = a3p;
    a3p->siguiente = a6;

    v4->AristaAdyacente = a4p;
    a4p->siguiente = a5p;
    a5p->siguiente = a6p;

    v5->AristaAdyacente = a2p;
    a2p->siguiente = a5;

    // VER RUTAS Y PESOS DE CADA VERTICE DE VERTICE EN ESPECIFICO : PRUEBA

    ListaVertices copia = listaVertices;
    int op;
    do
    {
        system("cls");

        std::cout << "\tINGRESE VERTICE A VER SUS VECINOS:\t\n"
                  << std::endl;
        std::cout << "presionar 0 para salir\n " << std::endl;
        int i = 1;
        do
        {
            std::cout << "OPCION " << i << ": " << copia->nombre << std::endl;
            copia = copia->siguiente;
            i++;

        } while (copia != listaVertices);
        std::cout << "\ningrese opcion -----> ";
        std::cin >> op;

        switch (op)
        {
        case 1:
            MostrarVertice(v1);
            break;
        case 2:
            MostrarVertice(v2);
            break;
        case 3:
            MostrarVertice(v3);
            break;
        case 4:
            MostrarVertice(v4);
            break;
        case 5:
            MostrarVertice(v5);
            break;
        default:
            std::cout << "OPCION INVALIDA" << std::endl;
        }
        system("pause");
    } while (op != 0);

    return 0;
}
// EJECUTAR PROGRAMA CON .\src\output\programa.exe