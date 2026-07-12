#pragma once
#include "grafo.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

struct EstadoPathfinder {
    vertice origen = nullptr;
    vertice destino = nullptr;
    std::vector<vertice> rutaDijkstra;
    bool mostrarRuta = false;
    bool filtrarAccesibilidad = false;

    vertice nodoSeleccionado = nullptr;
};

void inicializarVentana(vertice cabeza);
void ajustarVistaAVertices(sf::View &vista, const sf::Vector2u &windowSize, const std::vector<vertice> &verticesAMostrar, float &zoom);

void procesarSeleccionDeNodos(sf::RenderWindow &window, sf::Vector2f posicionMouse, vertice cabeza, EstadoPathfinder& estado);
vertice obtenerVerticePorClic(vertice cabeza, sf::Vector2f mousePosicion, float radio);

void dibujarNodos(sf::RenderWindow &window, vertice cabeza);
void dibujarRutaDijkstra(sf::RenderWindow &window, const std::vector<vertice> &ruta);