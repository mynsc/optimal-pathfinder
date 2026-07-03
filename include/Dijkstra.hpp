#pragma once
#include "vertice.hpp"
#include <vector>

// Alias solicitado por la firma del metodo: representa la cabeza de la
// lista enlazada de vertices (mismo tipo que "vertice", solo que aqui se
// usa semanticamente como "toda la lista", no como un vertice puntual).
typedef Vertice *ListaVertices;

/**
 * Calcula la ruta mas corta entre "origen" y "destino" usando el algoritmo
 * de Dijkstra, recorriendo la lista enlazada de vertices que comienza en "head".
 *
 * @param head                Cabeza de la lista enlazada de vertices del grafo.
 * @param origen               Vertice de partida.
 * @param destino              Vertice de llegada.
 * @param filtrarAccesibilidad Si es true, se descartan las aristas con esAccesible == false.
 *                             Las aristas con nivelRestriccion > 0 siempre se descartan,
 *                             sin importar el valor de este parametro.
 *
 * @return Vector con el camino ordenado desde origen hasta destino (ambos incluidos).
 *         Si no existe una ruta valida bajo las restricciones dadas, devuelve un vector vacio.
 */
std::vector<Vertice*> calcularRutaDijkstra(ListaVertices head, Vertice* origen, Vertice* destino, bool filtrarAccesibilidad);