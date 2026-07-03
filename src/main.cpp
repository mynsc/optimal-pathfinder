#include "grafo.hpp"
#include "dijkstra.hpp"

#include <iostream>
#include <vector>

// Imprime el resultado de calcularRutaDijkstra: la secuencia de vertices
// y el peso total acumulado de la ruta (recalculado recorriendo las aristas).
void imprimirRuta(const std::vector<vertice> &ruta)
{
    if (ruta.empty())
    {
        std::cout << "\nNo existe una ruta valida entre esos vertices con las restricciones indicadas\n";
        return;
    }

    int pesoTotal = 0;

    std::cout << "\nRuta encontrada:\n";
    for (size_t i = 0; i < ruta.size(); i++)
    {
        std::cout << ruta[i]->nombre;

        if (i != ruta.size() - 1)
        {
            // Buscar el peso de la arista entre ruta[i] y ruta[i + 1] para sumarlo
            arista actual = ruta[i]->aristaAdyacente;
            while (actual != nullptr)
            {
                if (actual->destino == ruta[i + 1])
                {
                    pesoTotal += actual->peso;
                    break;
                }
                actual = actual->siguiente;
            }

            std::cout << "  ->  ";
        }
    }

    std::cout << "\n\nPeso total de la ruta: " << pesoTotal << "\n";
}

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

    // Arreglo auxiliar para poder seleccionar vertices por numero en el menu
    const int TOTAL_VERTICES = 5;
    vertice vertices[TOTAL_VERTICES] = { v1, v2, v3, v4, v5 };

    int opcion = 0;
    do
    {
        std::cout << "\n===== MENU PRINCIPAL =====\n";
        std::cout << "1. Ver vecinos de un vertice\n";
        std::cout << "2. Calcular ruta mas corta (Dijkstra)\n";
        std::cout << "0. Salir\n";

        std::cout << "\nIngrese opcion >> ";
        std::cin >> opcion;

        switch (opcion)
        {
            case 1:
            {
                std::cout << "\n\tEscoja un vertice para visualizar sus vecinos\t\n\n";
                vertice actual = cabeza;
                int i = 1;
                while (actual != nullptr)
                {
                    std::cout << i << ". " << actual->nombre << std::endl;
                    actual = actual->siguiente;
                    i++;
                }
                std::cout << "0. Volver\n";
                std::cout << "\nIngrese opcion >> ";

                int sub = 0;
                std::cin >> sub;

                if (sub >= 1 && sub <= TOTAL_VERTICES)
                    mostrarVertices(vertices[sub - 1]);
                else if (sub != 0)
                    std::cout << "Opcion invalida\n";

                break;
            }

            case 2:
            {
                std::cout << "\n\tVertices disponibles\t\n\n";
                for (int j = 0; j < TOTAL_VERTICES; j++)
                    std::cout << (j + 1) << ". " << vertices[j]->nombre << std::endl;

                int indiceOrigen = 0, indiceDestino = 0;

                std::cout << "\nSeleccione el vertice de origen >> ";
                std::cin >> indiceOrigen;
                std::cout << "Seleccione el vertice de destino >> ";
                std::cin >> indiceDestino;

                if (indiceOrigen < 1 || indiceOrigen > TOTAL_VERTICES ||
                    indiceDestino < 1 || indiceDestino > TOTAL_VERTICES)
                {
                    std::cout << "Opcion invalida\n";
                break;
                }

                char respuesta;
                std::cout << "Filtrar solo rutas accesibles? (s/n) >> ";
                std::cin >> respuesta;

                bool filtrarAccesibilidad = (respuesta == 's' || respuesta == 'S');

                std::vector<vertice> ruta = calcularRutaDijkstra(
                    cabeza,
                    vertices[indiceOrigen - 1],
                    vertices[indiceDestino - 1],
                    filtrarAccesibilidad
                );

                imprimirRuta(ruta);

                break;
            }

            case 0:
                std::cout << "Saliendo del programa...\n";
                break;

            default:
                std::cout << "Opcion invalida\n";
        }
        
    } while (opcion != 0);

    liberarGrafo(cabeza);

    return 0;
}