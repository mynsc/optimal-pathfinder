#include "interfaz.hpp"

#include "dijkstra.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <cmath>
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

    vertice origen = nullptr;
    vertice destino = nullptr;
    std::vector<vertice> rutaDijkstra;
    bool filtrarAccesibilidad = false;
    bool mostrarRutaDijkstra = false;

    // Iniciar el bucle de la ventana
    while (window.isOpen())
    {
        // Procesar eventos
        while (const std::optional event = window.pollEvent())
        {
            // Cerrar ventana: Salir
            if (event->is<sf::Event::Closed>())
                window.close();

            // Detectar evento de click izquierdo del raton
            if (const auto* clickMouse = event->getIf<sf::Event::MouseButtonPressed>()) {

                if (clickMouse->button == sf::Mouse::Button::Left)
                {
                    // Obtener la posicion del raton relativa a la ventana
                    sf::Vector2f posicionMouse(static_cast<float>(clickMouse->position.x), 
                                               static_cast<float>(clickMouse->position.y));

                    vertice nodoSeleccionado = obtenerVerticePorClick(cabeza, posicionMouse, 5.f);

                    if (nodoSeleccionado != nullptr)
                    {
                        if (origen == nullptr) 
                        {
                            // Primer click: Asignar origen
                            origen = nodoSeleccionado;
                            std::cout << "Nodo origen: " << origen->nombre << "(ID: " << origen->id << ")\n";
                        } 
                        else if (destino == nullptr && nodoSeleccionado != origen) 
                        {
                            // Segundo click: Asignar destino y calcular ruta
                            destino = nodoSeleccionado;
                            std::cout << "Nodo destino: " << destino->nombre << "(ID: " << destino->id << ")\n";

                            rutaDijkstra = calcularRutaDijkstra(cabeza, origen, destino, filtrarAccesibilidad);

                            imprimirRuta(rutaDijkstra);

                            mostrarRutaDijkstra = true;
                        } 
                        else 
                        {
                            // Tercer click: Reiniciar la seleccion
                            origen = nodoSeleccionado;
                            std::cout << "Nuevo nodo origen: " << origen->nombre << "(ID: " << origen->id << ")\n";
                            destino = nullptr;
                            rutaDijkstra.clear();
                            mostrarRutaDijkstra = false;
                        }
                    }
                }
            }
        }

        // Limpiar ventana
        window.clear();

        window.draw(spriteMapa);

        if (mostrarRutaDijkstra)
            dibujarRutaDijkstra(window, rutaDijkstra);

        dibujarNodos(window, cabeza);

        // Actualizar ventana
        window.display();
    }
}

void dibujarNodos(sf::RenderWindow &window, vertice cabeza)
{
    vertice actual = cabeza;
    float radio = 5.f;
    
    // Configurar el circulo base
    sf::CircleShape nodoForma(radio);
    nodoForma.setFillColor(sf::Color::Green);
    nodoForma.setOutlineThickness(2.f);
    nodoForma.setOutlineColor(sf::Color::Black);

    while (actual != nullptr)
    {
        // SFML posiciona desde la esquina superior izquierda del shape, 
        // centramos restando el radio a las coordenadas
        nodoForma.setPosition(sf::Vector2f(actual->coordenadasX - radio, actual->coordenadasY - radio));
        
        window.draw(nodoForma);
        actual = actual->siguiente;
    }
}

void dibujarRutaDijkstra(sf::RenderWindow &window, const std::vector<vertice> &ruta)
{
    float grosor = 3.f;

    // Extraer los puntos del grafo
    std::vector<sf::Vector2f> puntos;
    for (int i = 0; i < ruta.size(); ++i)
        puntos.push_back(sf::Vector2f(ruta[i]->coordenadasX, ruta[i]->coordenadasY));

    if (puntos.empty()) return;

    sf::Color colorRuta = sf::Color::Red;

    // Dibujar los segmentos y las uniones
    for (int i = 0; i < puntos.size(); ++i)
    {
        // Si hay un siguiente punto, dibujamos el segmento rectangular
        if (i < puntos.size() - 1)
        {
            sf::Vector2f p1 = puntos[i];
            sf::Vector2f p2 = puntos[i + 1];

            sf::Vector2f direccion = p2 - p1;
            float longitud = direccion.length(); 

            if (longitud > 0.f)
            {
                sf::RectangleShape segmento({longitud, grosor});
                segmento.setFillColor(colorRuta);
                
                // Centrar el origen verticalmente respecto al inicio de la linea
                segmento.setOrigin({0.f, grosor / 2.f});
                segmento.setPosition(p1);

                // Obtener el angulo exacto desde el eje X
                sf::Angle angulo = direccion.angle();
                segmento.setRotation(angulo); 

                window.draw(segmento);
            }
        }
    }
}

vertice obtenerVerticePorClick(vertice cabeza, sf::Vector2f posicionMouse, float radio)
{
    vertice actual = cabeza;
    while (actual != nullptr)
    {
        // Calcular distancia euclidiana entre el clic y el nodo
        float dx = posicionMouse.x - actual->coordenadasX;
        float dy = posicionMouse.y - actual->coordenadasY;
        float distancia = std::sqrt(dx * dx + dy * dy);

        // Si el click cae dentro del radio del nodo, lo retornamos
        if (distancia <= radio) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return nullptr;
}