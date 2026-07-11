#pragma once
#include "grafo.hpp"

#include <vector>

/**
 * Calcula la ruta mas corta entre "origen" y "destino" usando el algoritmo
 * de Dijkstra, recorriendo la lista enlazada de vertices que comienza en "head".
 *
 * @param head                 Cabeza de la lista enlazada de vertices del grafo.
 * @param origen               Vertice de partida.
 * @param destino              Vertice de llegada.
 * @param filtrarAccesibilidad Si es true, se descartan las aristas con esAccesible == false.
 *                             Las aristas con nivelRestriccion > 0 siempre se descartan,
 *                             sin importar el valor de este parametro.
 *
 * @return Vector con el camino ordenado desde origen hasta destino (ambos incluidos).
 *         Si no existe una ruta valida bajo las restricciones dadas, devuelve un vector vacio.
 */
std::vector<vertice> calcularRutaDijkstra(vertice head, vertice origen, vertice destino, bool filtrarAccesibilidad);