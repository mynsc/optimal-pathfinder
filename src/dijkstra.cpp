#include "dijkstra.hpp"
#include "grafo.hpp"

#include <iostream>
#include <limits>

std::vector<vertice> calcularRutaDijkstra(vertice cabeza, vertice origen, vertice destino, bool filtrarAccesibilidad)
{
    std::vector<vertice> ruta;

    if (!cabeza || !origen || !destino) return ruta;

    // Contar cuantos vertices hay en la lista enlazada
    int totalVertices = 0;
    for (vertice actual = cabeza; actual != nullptr; actual = actual->siguiente) totalVertices++;

    // Reservar arreglos paralelos (mismo indice = mismo vertice)
    vertice *vertices = new vertice[totalVertices];
    int *distancia = new int[totalVertices];
    vertice *anterior = new vertice[totalVertices];
    bool *visitado = new bool[totalVertices];

    const int INFINITO = std::numeric_limits<int>::max();

    int i = 0;
    for (vertice actual = cabeza; actual != nullptr; actual = actual->siguiente, i++)
    {
        vertices[i] = actual;
        distancia[i] = INFINITO;
        anterior[i] = nullptr;
        visitado[i] = false;
    }

    // Ubicar el indice del origen dentro del arreglo
    int indiceOrigen = -1;
    for (int j = 0; j < totalVertices; j++)
    {
        if (vertices[j] == origen)
        {
            indiceOrigen = j;
            break;
        }
    }

    if (indiceOrigen == -1)
    {
        delete[] vertices;
        delete[] distancia;
        delete[] anterior;
        delete[] visitado;
        return ruta; // el origen no pertenece a esta lista de vertices
    }

    distancia[indiceOrigen] = 0;

    // Bucle principal de Dijkstra
    for (int i = 0; i < totalVertices; i++)
    {
        // Buscar el vertice no visitado con menor distancia
        int indiceActual = -1;
        int menor = INFINITO;

        for (int j = 0; j < totalVertices; j++)
        {
            if (!visitado[j] && distancia[j] < menor)
            {
                menor = distancia[j];
                indiceActual = j;
            }
        }

        // Si no queda ningun vertice alcanzable, terminamos
        if (indiceActual == -1) break;

        visitado[indiceActual] = true;
        vertice verticeActual = vertices[indiceActual];

        if (verticeActual == destino) break;

        // Recorrer las aristas adyacentes al vertice actual (lista enlazada de aristas)
        for (arista arista = verticeActual->aristaAdyacente; arista != nullptr; arista = arista->siguiente)
        {
            // --- Condicional de accesibilidad/restriccion ---
            // Se ignora la arista por completo si:
            //   a) se pidio filtrar por accesibilidad y la arista no es accesible, o
            //   b) la arista tiene algun nivel de restriccion (>0), sin importar el filtro.
            if (filtrarAccesibilidad && !arista->esAccesible) continue;
            if (arista->nivelRestriccion > 0) continue;

            vertice vecino = arista->destino;

            // Buscar el indice del vecino dentro del arreglo de vertices
            int indiceVecino = -1;
            for (int j = 0; j < totalVertices; j++)
            {
                if (vertices[j] == vecino)
                {
                    indiceVecino = j;
                    break;
                }
            }

            if (indiceVecino == -1 || visitado[indiceVecino]) continue;

            int nuevaDistancia = distancia[indiceActual] + arista->peso;

            if (nuevaDistancia < distancia[indiceVecino])
            {
                distancia[indiceVecino] = nuevaDistancia;
                anterior[indiceVecino] = verticeActual;
            }
        }
    }

    // Ubicar el indice del destino y verificar si fue alcanzado
    int indiceDestino = -1;
    for (int j = 0; j < totalVertices; j++)
    {
        if (vertices[j] == destino)
        {
            indiceDestino = j;
            break;
        }
    }

    if (indiceDestino == -1 || distancia[indiceDestino] == INFINITO)
    {
        delete[] vertices;
        delete[] distancia;
        delete[] anterior;
        delete[] visitado;
        return ruta; // no existe ruta valida bajo estas restricciones
    }

    // Reconstruir la ruta caminando hacia atras con el arreglo "anterior".
    // Primero se cuenta el largo del camino, luego se llena de atras hacia adelante.
    int largoRuta = 0;
    for (vertice v = destino; v != nullptr;)
    {
        largoRuta++;
        if (v == origen) break;

        int indice = -1;
        for (int j = 0; j < totalVertices; j++)
        {
            if (vertices[j] == v)
            {
                indice = j;
                break;
            }
        }

        v = anterior[indice];
    }

    ruta.resize(largoRuta);

    int posicion = largoRuta - 1;
    vertice v = destino;
    while (v != nullptr)
    {
        ruta[posicion--] = v;
        if (v == origen) break;

        int indice = -1;
        for (int j = 0; j < totalVertices; j++)
        {
            if (vertices[j] == v)
            {
                indice = j;
                break;
            }
        }

        v = anterior[indice];
    }

    delete[] vertices;
    delete[] distancia;
    delete[] anterior;
    delete[] visitado;

    return ruta;
}

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