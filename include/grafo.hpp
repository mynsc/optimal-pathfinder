#pragma once
#include "vertice.hpp"
#include "arista.hpp"
#include <string>

// crear y gestionar vertices
ListaVertices crearVertice(const std::string &nombre);
void agregarVertice(ListaVertices &head, ListaVertices v);
ListaVertices buscarVertice(ListaVertices head, const std::string &nombre);

// crear y gestionar aristas (lista de adyacencia: cabeza ligada)
ListaAristas crearArista(ListaVertices origen, ListaVertices destino, short peso);
void agregarArista(ListaVertices origen, ListaAristas arista);
void enlaceBidireccional(ListaVertices a, ListaVertices b, short peso);

// liberar memoria
void liberarGrafo(ListaVertices &head);