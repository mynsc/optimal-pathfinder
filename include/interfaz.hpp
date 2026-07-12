#pragma once
#include "grafo.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

void inicializarVentana(vertice cabeza);

vertice obtenerVerticePorClick(vertice cabeza, sf::Vector2f mousePosicion, float radio);

void dibujarNodos(sf::RenderWindow &window, vertice cabeza);
void dibujarRutaDijkstra(sf::RenderWindow &window, const std::vector<vertice> &ruta);