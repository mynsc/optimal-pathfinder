#pragma once
#include "grafo.hpp"

#include <SFML/Graphics.hpp>

void inicializarVentana(vertice cabeza);

void dibujarNodos(sf::RenderWindow &window, vertice cabeza);
vertice obtenerVerticePorClick(vertice cabeza, sf::Vector2f mousePosicion, float radio);