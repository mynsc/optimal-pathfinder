#pragma once
#include "grafo.hpp"

#include <SFML/Graphics.hpp>

void inicializarVentana(vertice cabeza);

void dibujarNodos(sf::RenderWindow &window, vertice cabeza);
void dibujarRuta(sf::RenderWindow &window, vertice ruta);