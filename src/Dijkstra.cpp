#include "../include/dijkstra.hpp"
#include "../include/arista.hpp"

#include <limits>

std::vector<Vertice *> calcularRutaDijkstra(ListaVertices head, Vertice *origen, Vertice *destino, bool filtrarAccesibilidad)
{
    std::vector<Vertice *> ruta;

    if (!head || !origen || !destino)
        return ruta;

    // Contar cuantos vertices hay en la lista enlazada
    int totalVertices = 0;
    for (Vertice *actual = head; actual != nullptr; actual = actual->siguiente)
        totalVertices++;

    // Reservar arreglos paralelos con "new" (mismo indice = mismo vertice)
    Vertice **vertices = new Vertice *[totalVertices];
    int *distancia = new int[totalVertices];
    Vertice **anterior = new Vertice *[totalVertices];
    bool *visitado = new bool[totalVertices];

    const int INFINITO = std::numeric_limits<int>::max();

    int i = 0;
    for (Vertice *actual = head; actual != nullptr; actual = actual->siguiente, i++)
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

    //Bucle principal de Dijkstra (version O(V^2), sin cola de prioridad)
    for (int iter = 0; iter < totalVertices; iter++)
    {
        // 4a) Buscar el vertice no visitado con menor distancia
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
        if (indiceActual == -1)
            break;

        visitado[indiceActual] = true;
        Vertice *verticeActual = vertices[indiceActual];

        if (verticeActual == destino)
            break;

        // Recorrer las aristas adyacentes al vertice actual (lista enlazada de aristas)
        for (Arista *arista = verticeActual->aristaAdyacente; arista != nullptr; arista = arista->siguiente)
        {
            // --- Condicional critico de accesibilidad/restriccion ---
            // Se ignora la arista por completo si:
            //   a) se pidio filtrar por accesibilidad y la arista no es accesible, o
            //   b) la arista tiene algun nivel de restriccion (>0), sin importar el filtro.
            if (filtrarAccesibilidad && !arista->esAccesible)
                continue;
            if (arista->nivelRestriccion > 0)
                continue;

            Vertice *vecino = arista->destino;

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

            if (indiceVecino == -1 || visitado[indiceVecino])
                continue;

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
    //    Primero se cuenta el largo del camino, luego se llena de atras hacia adelante.
    int largoRuta = 0;
    for (Vertice *v = destino; v != nullptr;)
    {
        largoRuta++;
        if (v == origen)
            break;

        int idx = -1;
        for (int j = 0; j < totalVertices; j++)
        {
            if (vertices[j] == v)
            {
                idx = j;
                break;
            }
        }

        v = anterior[idx];
    }

    ruta.resize(largoRuta);

    int pos = largoRuta - 1;
    for (Vertice *v = destino; v != nullptr;)
    {
        ruta[pos--] = v;
        if (v == origen)
            break;

        int idx = -1;
        for (int j = 0; j < totalVertices; j++)
        {
            if (vertices[j] == v)
            {
                idx = j;
                break;
            }
        }

        v = anterior[idx];
    }

    delete[] vertices;
    delete[] distancia;
    delete[] anterior;
    delete[] visitado;

    return ruta;
}