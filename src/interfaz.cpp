#include "interfaz.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

void inicializarVentana()
{
    // Crear la ventana principal
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Optimal PathFinder - UNMSM");

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

        // Actualizar ventana
        window.display();
    }
}