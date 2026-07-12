#pragma once
#include "grafo.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

void inicializarVentana(vertice cabeza);
void ajustarVistaAVertices(sf::View &vista, const sf::Vector2u &windowSize, const std::vector<vertice> &verticesAMostrar, float &zoom);

void detectarEventoClicIzquierdo(const std::optional<sf::Event> event, vertice cabeza, std::vector<vertice> rutaDijkstra, bool mostrarRutaDijkstra);
vertice obtenerVerticePorClick(vertice cabeza, sf::Vector2f mousePosicion, float radio);

void dibujarNodos(sf::RenderWindow &window, vertice cabeza);
void dibujarRutaDijkstra(sf::RenderWindow &window, const std::vector<vertice> &ruta);