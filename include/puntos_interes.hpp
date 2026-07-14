#pragma once
#include <SFML/Graphics.hpp>

// Estructura del nodo para la lista circular
struct NodoPuntoInteres {
    sf::Vector2f coordenadas;
    float zoomIdeal;
    NodoPuntoInteres* siguiente;
};

typedef NodoPuntoInteres* nodoPuntoInteres;

nodoPuntoInteres crearNodoPunto(sf::Vector2f coordenadas, float zoom);
void agregarPuntoCircular(nodoPuntoInteres &cabeza, sf::Vector2f coordenadas, float zoom);
nodoPuntoInteres inicializarPuntosInteres();
void liberarPuntosInteres(nodoPuntoInteres &cabeza);