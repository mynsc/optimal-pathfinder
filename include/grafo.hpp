#pragma once
#include "vertice.hpp"
#include "arista.hpp"
#include <string>

vertice crearVertice(const std::string &nombre);
void agregarVertice(vertice &cabeza, vertice v);
vertice buscarVertice(vertice cabeza, const std::string &nombre);

arista crearArista(vertice origen, vertice destino, short peso);
void agregarArista(vertice origen, arista arista);
void enlaceBidireccional(vertice a, vertice b, short peso);

void liberarGrafo(vertice &cabeza);