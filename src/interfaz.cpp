#include "interfaz.hpp"

#include "dijkstra.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <cmath>
#include <iostream>

void inicializarVentana(vertice cabeza)
{
    // Crear la ventana principal
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Optimal PathFinder - UNMSM");
    
    // Evitar encoger la ventana menos de 500 x 500
    window.setMinimumSize(sf::Vector2u({500, 500}));

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
    float zoomActual = 1.f;

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
            if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                // Calcular las nuevas dimensiones
                sf::Vector2f newWindowSize(resized->size);

                // Mantener el nivel de zoom actual proporcional a la nueva ventana
                vista.setSize({newWindowSize.x * zoomActual, newWindowSize.y * zoomActual});

                // Asignar nuevas dimensiones a la ventana
                window.setView(vista);
            }

            // Detectar cuando se presiona un boton del raton
            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>())
            {
                // Boton derecho para arrastrar
                if (click->button == sf::Mouse::Button::Right)
                {
                    arrastrando = true;
                    posicionInicioArrastre = sf::Vector2f(static_cast<float>(click->position.x),
                                                          static_cast<float>(click->position.y));
                    centroInicio = vista.getCenter();
                }
                
                // Boton izquierdo para calcular ruta con Dijkstra
                else if (click->button == sf::Mouse::Button::Left)
                {
                    // Convertir el pixel de la pantalla a coordenadas del mapa real
                    sf::Vector2f posicionMundo = window.mapPixelToCoords(click->position, vista);

                    procesarSeleccionDeNodos(window, posicionMundo, cabeza, estado);
                }
            }

            // Detectar cuando se suelta un boton del raton
            else if (const auto* releaseMouse = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (releaseMouse->button == sf::Mouse::Button::Right) arrastrando = false;
            }

            // Detectar cuando el raton se mueve
            else if (const auto* movement = event->getIf<sf::Event::MouseMoved>())
            {
                if (arrastrando)
                {
                    // Obtener la posicion fisica actual del raton
                    sf::Vector2i posicionFisica = movement->position;

                    // Convertirla a coordenadas del mundo usando la vista actual
                    sf::Vector2f posicionActual = window.mapPixelToCoords(posicionFisica, vista);
                    sf::Vector2f posicionInicio = window.mapPixelToCoords(sf::Vector2i(posicionInicioArrastre), vista);

                    // Calcular el delta en coordenadas de mundo
                    sf::Vector2f delta = posicionActual - posicionInicio;

                    // Mover la vista
                    vista.move(-delta);
                    window.setView(vista);

                    // Actualizar la posición de inicio
                    posicionInicioArrastre = sf::Vector2f(posicionFisica);
                }
            }

            // Detectar cuando se gira la rueda del raton
            else if (const auto *rueda = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if (rueda->wheel == sf::Mouse::Wheel::Vertical)
                {
                    manejarZoomRaton(window, vista, rueda, zoomActual);
                }
            }
        }
    

        // Limpiar ventana
        window.clear();

        window.draw(spriteMapa);

        if (estado.mostrarRuta)
            dibujarRutaDijkstra(window, estado.rutaDijkstra);

        dibujarNodos(window, cabeza);

        if (estado.mostrarRuta)
        {
            int i = 0;
            float radio = 8.f;

            for (vertice v: estado.rutaDijkstra)
            {
                sf::CircleShape resaltado;
                resaltado.setRadius(radio);

                // Colorear vertice de origen
                if (i == 0)
                    resaltado.setFillColor(sf::Color(0, 100, 255, 100));

                // Colorear vertice de destino
                else if (i == estado.rutaDijkstra.size() - 1)
                    resaltado.setFillColor(sf::Color(0, 220, 0, 100));

                // Colorear vertices intermedios de la ruta
                else
                    resaltado.setFillColor(sf::Color(255, 50, 50, 100));

                resaltado.setPosition(sf::Vector2f(v->coordenadasX - radio, v->coordenadasY - radio));
                window.draw(resaltado);

                i++;
            }
        }

        if (estado.nodoSeleccionado != nullptr)
        {
            // Configurar forma para resaltado del nodo seleccionado
            float radio = 14.f;

            sf::CircleShape nodoSeleccionadoForma(radio);
            nodoSeleccionadoForma.setFillColor(sf::Color(255, 255, 255, 80));
            nodoSeleccionadoForma.setOutlineThickness(2.f);
            nodoSeleccionadoForma.setOutlineColor(sf::Color::Yellow);
            nodoSeleccionadoForma.setPosition(sf::Vector2f(
                estado.nodoSeleccionado->coordenadasX - radio, 
                estado.nodoSeleccionado->coordenadasY - radio
            ));
                
            window.draw(nodoSeleccionadoForma);
        }

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
    float factorNecesario = std::max({1.f, anchoNecesario / anchoVentana, altoNecesario / altoVentana});

    vista.setSize(sf::Vector2f(anchoVentana * factorNecesario, altoVentana * factorNecesario));
    vista.setCenter(sf::Vector2f((minX + maxX) / 2.0f, (minY + maxY) / 2.0f));

    zoom = factorNecesario;
}

void procesarSeleccionDeNodos(sf::RenderWindow &window, sf::Vector2f posicionMouse, vertice cabeza, EstadoPathfinder& estado)
{
    vertice nodoSeleccionado = obtenerVerticePorClic(cabeza, posicionMouse, 5.f);

    if (nodoSeleccionado == nullptr) return;

    estado.nodoSeleccionado = nodoSeleccionado;

    // Primer clic: Asignar origen
    if (estado.origen == nullptr) 
    {
        estado.origen = nodoSeleccionado;
        std::cout << "Nodo origen: " << estado.origen->nombre << " (ID: " << estado.origen->id << ")\n";
    } 

    // Segundo clic: Asignar destino y calcular ruta
    else if (estado.destino == nullptr && nodoSeleccionado != estado.origen) 
    {
        estado.destino = nodoSeleccionado;
        std::cout << "Nodo destino: " << estado.destino->nombre << " (ID: " << estado.destino->id << ")\n";
        estado.rutaDijkstra = calcularRutaDijkstra(cabeza, estado.origen, estado.destino, estado.filtrarAccesibilidad);
        imprimirRuta(estado.rutaDijkstra);
        estado.mostrarRuta = true;
    }

    // Tercer clic: Reiniciar la seleccion
    else 
    {
        estado.origen = nodoSeleccionado;
        std::cout << "Nuevo nodo origen: " << estado.origen->nombre << " (ID: " << estado.origen->id << ")\n";
        estado.destino = nullptr;
        estado.rutaDijkstra.clear();
        estado.mostrarRuta = false;
    }
}

vertice obtenerVerticePorClic(vertice cabeza, sf::Vector2f posicionMouse, float radio)
{
    vertice actual = cabeza;
    while (actual != nullptr)
    {
        // Calcular distancia euclidiana entre el clic y el nodo
        float dx = posicionMouse.x - actual->coordenadasX;
        float dy = posicionMouse.y - actual->coordenadasY;
        float distancia = std::sqrt(dx * dx + dy * dy);

        // Si el clic cae dentro del radio del nodo, lo retornamos
        if (distancia <= radio)
        {
            return actual;
        }

        actual = actual->siguiente;
    }
    return nullptr;
}

void manejarZoomRaton(sf::RenderWindow &window, sf::View &vista, const sf::Event::MouseWheelScrolled *rueda, float &zoomActual)
{
    // Calcular factor base
    float factorZoom = 1.f - rueda->delta * 0.1f;
    float nuevoZoom = zoomActual * factorZoom;

    // Limitar el zoom exacto
    if (nuevoZoom < 1.f) 
    {
        // Si nos pasamos hacia abajo, calculamos el factor justo para llegar a 1.f
        factorZoom = 1.f / zoomActual;
        zoomActual = 1.f;
    } 
    else if (nuevoZoom > 5.f) 
    {
        // Si nos pasamos hacia arriba, calculamos el factor justo para llegar a 5.f
        factorZoom = 5.f / zoomActual;
        zoomActual = 5.f;
    }
    else
    {
        zoomActual = nuevoZoom;
    }

    // Aplicar transformaciones SOLO si el zoom realmente va a cambiar
    // Si factorZoom es 1.f, significa que ya estamos en el limite y bloqueamos la camara
    if (factorZoom != 1.f)
    {
        if (rueda->delta > 0) 
        {
            // Acercar hacia el cursor
            sf::Vector2i pixelPos(rueda->position.x, rueda->position.y);
            sf::Vector2f posMundoAntes = window.mapPixelToCoords(pixelPos, vista);
            
            vista.zoom(factorZoom);
            
            sf::Vector2f posMundoDespues = window.mapPixelToCoords(pixelPos, vista);
            vista.move(posMundoAntes - posMundoDespues);
        }
        else 
        {
            // Alejar desde el centro sin seguir el raton
            vista.zoom(factorZoom);
        }

        // Aplicar la vista actualizada
        window.setView(vista);
    }
}

void dibujarNodos(sf::RenderWindow &window, vertice cabeza)
{
    vertice actual = cabeza;
    float radio = 8.f;
    sf::Color color(180, 180, 200);
    
    // Configurar el circulo base
    sf::CircleShape nodoForma(radio);
    nodoForma.setFillColor(color);
    nodoForma.setOutlineThickness(2.f);
    nodoForma.setOutlineColor(sf::Color::White);

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