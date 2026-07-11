#include "interfaz.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

void inicializarVentana(vertice cabeza)
{
    // Crear la ventana principal
    sf::RenderWindow window(sf::VideoMode({389, 487}), "Optimal PathFinder - UNMSM");

    // Cargar la imagen del icono
    sf::Image icono;
    if (!icono.loadFromFile("assets/logo.png")) return;

    // Establecer el icono en la ventana
    window.setIcon(icono.getSize(), icono.getPixelsPtr());

    // Cargar la textura del mapa desde un archivo
    sf::Texture texturaMapa;
    if (!texturaMapa.loadFromFile("assets/mapa.png")) return;

    // Crear el sprite del mapa y asignarle la textura
    sf::Sprite spriteMapa(texturaMapa);

    // Escalar el sprite del mapa para que cubra toda la ventana
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = texturaMapa.getSize();

    // Calculos matematicos para la escala de la textura
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;

    // Iniciar el bucle de la ventana
    while (window.isOpen())
    {
        // Procesar eventos
        while (const std::optional event = window.pollEvent())
        {
            // Cerrar ventana: Salir
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Limpiar ventana
        window.clear();

        window.draw(spriteMapa);
        dibujarNodos(window, cabeza);

        // Actualizar ventana
        window.display();
    }
}

void dibujarNodos(sf::RenderWindow &window, vertice cabeza)
{
    vertice actual = cabeza;
    
    // Configurar el circulo base
    sf::CircleShape nodoForma(5.f); // Radio de 10 pixeles
    nodoForma.setFillColor(sf::Color::Green);
    nodoForma.setOutlineThickness(2.f);
    nodoForma.setOutlineColor(sf::Color::Black);

    while (actual != nullptr)
    {
        // SFML posiciona desde la esquina superior izquierda del shape, 
        // centramos restando el radio a las coordenadas
        nodoForma.setPosition(sf::Vector2f(actual->coordenadasX - 10.f, actual->coordenadasY - 10.f));
        
        window.draw(nodoForma);
        actual = actual->siguiente;
    }
}