#include "puntos_interes.hpp"

#include <iostream>

nodoPuntoInteres crearNodoPunto(sf::Vector2f coords, float zoom)
{
    nodoPuntoInteres nuevo = new NodoPuntoInteres();
    nuevo->coordenadas = coords;
    nuevo->zoomIdeal = zoom;
    nuevo->siguiente = nullptr;
    return nuevo;
}

void agregarPuntoCircular(nodoPuntoInteres &cabeza, sf::Vector2f coords, float zoom)
{
    nodoPuntoInteres nuevo = crearNodoPunto(coords, zoom);

    // Si la lista esta vacia, el nodo se apunta a si mismo
    if (cabeza == nullptr)
    {
        cabeza = nuevo;
        cabeza->siguiente = cabeza;
    }
    
    else
    {
        nodoPuntoInteres actual = cabeza;

        // Recorrer hasta encontrar el ultimo nodo
        while (actual->siguiente != cabeza) actual = actual->siguiente;

        // Insertar el nuevo nodo al final y cerrar el circulo
        actual->siguiente = nuevo;
        nuevo->siguiente = cabeza;
    }
}

nodoPuntoInteres inicializarPuntosInteres()
{
    nodoPuntoInteres cabeza = nullptr;

    // Cargar 4 coordenadas estrategicas fijas

    // Puerta 1
    agregarPuntoCircular(cabeza, sf::Vector2f(1188.f, 255.f), 2.5f);

    // Comedor Universitario
    agregarPuntoCircular(cabeza, sf::Vector2f(685.f, 432.f), 3.0f);

    // Facultad de Ingenieria de Sistemas e Informatica (FISI)
    agregarPuntoCircular(cabeza, sf::Vector2f(897.f, 1509.f), 3.5f);

    // Estadio San Marcos
    agregarPuntoCircular(cabeza, sf::Vector2f(561.f, 1141.f), 3.0f);

    return cabeza;
}

void liberarPuntosInteres(nodoPuntoInteres &cabeza)
{
    if (cabeza == nullptr) return;

    nodoPuntoInteres actual = cabeza->siguiente;

    // Recorrer y liberar hasta dar toda la vuelta
    while (actual != cabeza)
    {
        nodoPuntoInteres temp = actual;
        actual = actual->siguiente;
        delete temp;
    }

    // Liberar la cabeza al final y anular el puntero
    delete cabeza;
    cabeza = nullptr;
}
