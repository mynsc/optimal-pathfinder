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

    // Obtener dimensiones de la ventana y el mapa
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = texturaMapa.getSize();

    sf::View vista(sf::FloatRect(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(
            static_cast<float>(windowSize.x), 
            static_cast<float>(windowSize.y)
        )
    ));

    EstadoPathfinder estado;

    // Variables de control para la camara
    bool arrastrando = false;
    sf::Vector2f posicionInicioArrastre;
    sf::Vector2f centroInicio;

    // Iniciar el bucle de la ventana
    while (window.isOpen())
    {
        // Procesar eventos
        while (const std::optional event = window.pollEvent())
        {
            // Cerrar ventana: Salir
            if (event->is<sf::Event::Closed>())
                window.close();

            // Detectar evento de maximizar ventana
            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                // Calcular las nuevas dimensiones
                sf::Vector2f newWindowSize(resized->size);

                // Actualizar vista
                vista.setSize(newWindowSize);

                // Asignar nuevas dimensiones a la ventana
                window.setView(vista);
            }

            // Detectar cuando se presiona un boton del mouse
            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                // Boton derecho para arrastrar
                if (click->button == sf::Mouse::Button::Right)
                {
                    arrastrando = true;
                    posicionInicioArrastre = sf::Vector2f(static_cast<float>(click->position.x),
                                                     static_cast<float>(click->position.y));
                    centroInicio = vista.getCenter();
                }
                
                // Boton izquierdo para calcular ruta con Dijkstra
                detectarEventoClicIzquierdo(click, cabeza, estado);
            }

            // Detectar cuando se suelta un boton del mouse
            else if (const auto* releaseMouse = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (releaseMouse->button == sf::Mouse::Button::Right) arrastrando = false;
            }

            // Detectar cuando el mouse se mueve
            else if (const auto* movement = event->getIf<sf::Event::MouseMoved>())
            {
                if (arrastrando)
                {
                    // Calcular diferencia de posicion fisica en la pantalla
                    sf::Vector2f posicionActual(static_cast<float>(movement->position.x),
                                           static_cast<float>(movement->position.y));
                    sf::Vector2f delta = posicionActual - posicionInicioArrastre;

                    // Desplazar la camara
                    vista.move(-delta);
                    
                    // Notificar a la ventana que la vista ha cambiado
                    window.setView(vista);

                    // Actualizar posicion de inicio para el proximo fotograma de movimiento
                    posicionInicioArrastre = posicionActual;
                }
            }
        }

        // Limpiar ventana
        window.clear();

        window.draw(spriteMapa);

        if (estado.mostrarRuta)
            dibujarRutaDijkstra(window, estado.rutaDijkstra);

        dibujarNodos(window, cabeza);

        // Actualizar ventana
        window.display();
    }
}

void ajustarVistaAVertices(sf::View &vista, const sf::Vector2u &windowSize,
                           const std::vector<vertice> &verticesAMostrar,
                           float &zoom)
{
    float margen = 80.f;

    if (verticesAMostrar.empty()) return;

    float minX = verticesAMostrar[0]->coordenadasX;
    float maxX = verticesAMostrar[0]->coordenadasX;
    float minY = verticesAMostrar[0]->coordenadasY;
    float maxY = verticesAMostrar[0]->coordenadasY;

    for (vertice v : verticesAMostrar)
    {
        minX = std::min(minX, (v->coordenadasX));
        maxX = std::max(maxX, (v->coordenadasX));
        minY = std::min(minY, (v->coordenadasY));
        maxY = std::max(maxY, (v->coordenadasY));
    }

    float anchoNecesario = (maxX - minX) + margen * 2.0f;
    float altoNecesario = (maxY - minY) + margen * 2.0f;

    float anchoVentana = static_cast<float>(windowSize.x);
    float altoVentana = static_cast<float>(windowSize.y);

    // Nunca bajar de zoom 1:1
    float factorNecesario = std::max({1.0f, anchoNecesario / anchoVentana, altoNecesario / altoVentana});

    vista.setSize(sf::Vector2f(anchoVentana * factorNecesario, altoVentana * factorNecesario));
    vista.setCenter(sf::Vector2f((minX + maxX) / 2.0f, (minY + maxY) / 2.0f));

    zoom = factorNecesario;
}

void detectarEventoClicIzquierdo(const sf::Event::MouseButtonPressed* click, vertice cabeza, EstadoPathfinder& estado)
{
    if (click->button == sf::Mouse::Button::Left)
    {
        // Obtener la posicion del raton relativa a la ventana
        sf::Vector2f posicionMouse(static_cast<float>(click->position.x), 
                                    static_cast<float>(click->position.y));

        vertice nodoSeleccionado = obtenerVerticePorClic(cabeza, posicionMouse, 5.f);

        if (nodoSeleccionado != nullptr)
        {
            if (estado.origen == nullptr) 
            {
                // Primer click: Asignar origen
                estado.origen = nodoSeleccionado;
                std::cout << "Nodo origen: " << estado.origen->nombre << "(ID: " << estado.origen->id << ")\n";
            } 
            else if (estado.destino == nullptr && nodoSeleccionado != estado.origen) 
            {
                // Segundo click: Asignar destino y calcular ruta
                estado.destino = nodoSeleccionado;
                std::cout << "Nodo destino: " << estado.destino->nombre << "(ID: " << estado.destino->id << ")\n";

                estado.rutaDijkstra = calcularRutaDijkstra(cabeza, estado.origen, estado.destino, estado.filtrarAccesibilidad);

                imprimirRuta(estado.rutaDijkstra);

                estado.mostrarRuta = true;
            } 
            else 
            {
                // Tercer click: Reiniciar la seleccion
                estado.origen = nodoSeleccionado;
                std::cout << "Nuevo nodo origen: " << estado.origen->nombre << "(ID: " << estado.origen->id << ")\n";
                estado.destino = nullptr;
                estado.rutaDijkstra.clear();
                estado.mostrarRuta = false;
            }
        }
    }
}

vertice obtenerVerticePorClic(vertice cabeza, sf::Vector2f posicionMouse, float radio)
{
    vertice actual = cabeza;
    while (actual != nullptr)
    {
        // Calcular distancia euclidiana entre el click y el nodo
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