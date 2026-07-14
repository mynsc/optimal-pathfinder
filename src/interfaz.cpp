#include "interfaz.hpp"

#include "dijkstra.hpp"
#include "grafo.hpp"
#include "huffman.hpp"
#include "puntos_interes.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

void inicializarVentana(vertice cabeza)
{
    // Crear la ventana principal
    sf::RenderWindow window(sf::VideoMode({720, 720}), "Optimal PathFinder - UNMSM", sf::Style::Close);
    
    // Evitar encoger la ventana menos de 720 x 720
    window.setMinimumSize(sf::Vector2u({720, 720}));

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

    // Convertir escala a la escala real de SFML
    float zoomActual = 5.f;
    float escalaVisual = zoomActual / 5.f;
    vista.setSize(sf::Vector2f(windowSize.x * escalaVisual, windowSize.y * escalaVisual));

    // Centrar la camara en medio del mapa al iniciar
    vista.setCenter(sf::Vector2f(textureSize.x / 2.f, textureSize.y / 2.f));
    window.setView(vista);

    EstadoPathfinder estado;
    nodoPuntoInteres puntosInteres = inicializarPuntosInteres();
    nodoPuntoInteres puntoActual = puntosInteres;

    // Variables de control para la camara
    bool arrastrando = false;
    sf::Vector2f posicionInicioArrastre;
    sf::Vector2f centroInicio;

    // Variables para el carrusel
    sf::Vector2f centroObjetivo = vista.getCenter();
    float zoomObjetivo = zoomActual;
    sf::Clock relojCine; 
    float velocidadTransicion = 5.f;

    // Iniciar el bucle de la ventana
    while (window.isOpen())
    {
        // Procesar eventos
        while (const std::optional event = window.pollEvent())
        {
            // Cerrar ventana: Salir
            if (event->is<sf::Event::Closed>())
                window.close();

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

                    sf::Vector2f centro = vista.getCenter();
                    sf::Vector2f viewSize = vista.getSize();

                    // Limites en X: La mitad del ancho de la vista
                    float minX = viewSize.x / 2.f;
                    float maxX = static_cast<float>(textureSize.x) - minX;

                    // Limites en Y: La mitad del alto de la vista
                    float minY = viewSize.y / 2.f;
                    float maxY = static_cast<float>(textureSize.y) - minY;

                    // Forzar matematicamente a que el maximo nunca sea inferior al minimo
                    float seguroMaxX = std::max(minX, maxX);
                    float seguroMaxY = std::max(minY, maxY);

                    // Validar X: Si el zoom hace la vista mas grande que el mapa, la centramos
                    if (minX > maxX) centro.x = static_cast<float>(textureSize.x) / 2.f;
                    else centro.x = std::clamp(centro.x, minX, seguroMaxX);

                    // Validar Y: Si el zoom hace la vista mas grande que el mapa, la centramos
                    if (minY > maxY) centro.y = static_cast<float>(textureSize.y) / 2.f;
                    else centro.y = std::clamp(centro.y, minY, seguroMaxY);

                    // Aplicar la pocicion corregida
                    vista.setCenter(centro);

                    window.setView(vista);

                    // Actualizar la posición de inicio
                    posicionInicioArrastre = sf::Vector2f(posicionFisica);

                    // Sincronizar el objetivo del Lerp con la nueva posición manual
                    centroObjetivo = centro;
                }
            }

            // Detectar cuando se gira la rueda del raton
            else if (const auto *rueda = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if (rueda->wheel == sf::Mouse::Wheel::Vertical)
                {
                    manejarZoomRaton(window, vista, rueda, zoomActual, textureSize);

                    // Sincronizar el Lerp tras hacer zoom manual
                    centroObjetivo = vista.getCenter();
                    zoomObjetivo = zoomActual;
                }
            }

            // Detectar pulsaciones de teclado
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) 
            {
                // Guardar ruta: Tecla 'S'
                if (key->code == sf::Keyboard::Key::S) 
                {
                    guardarRutaValida(estado);
                }
                
                // Cargar ruta: Tecla 'L'
                else if (key->code == sf::Keyboard::Key::L) 
                {
                    cargarRutaGuardada(estado, cabeza);
                }

                // Navegar circularmente: Tecla 'Tab'
                else if (key->code == sf::Keyboard::Key::Tab) 
                {
                    if (puntoActual != nullptr) 
                    {
                        // Avanzar al siguiente nodo de la lista circular
                        puntoActual = puntoActual->siguiente;

                        // Extraer los valores y asignarlos a las variables objetivo
                        centroObjetivo = puntoActual->coordenadas;
                        zoomObjetivo = puntoActual->zoomIdeal;
                    }
                }
            }
        }

        // Interpolacion
        // Calcular el tiempo transcurrido desde el ultimo fotograma
        float dt = relojCine.restart().asSeconds();

        sf::Vector2f centroActual = vista.getCenter();
        bool vistaCambiada = false;

        // Calcular las diferencias absolutas
        float diffX = centroObjetivo.x - centroActual.x;
        float diffY = centroObjetivo.y - centroActual.y;
        float diffZoom = zoomObjetivo - zoomActual;

        // Lerp para la posicion
        if (std::abs(diffX) > 0.5f || std::abs(diffY) > 0.5f) // Si la distancia es significativa
        {
            centroActual.x += diffX * velocidadTransicion * dt;
            centroActual.y += diffY * velocidadTransicion * dt;
            vista.setCenter(centroActual);
            vistaCambiada = true;
        }
        else if (std::abs(diffX) > 0.0f || std::abs(diffY) > 0.0f) 
        {
            // Distancia insignificante: Forzar la posicion final exacta para evitar microtemblores
            vista.setCenter(centroObjetivo);
            vistaCambiada = true;
        }

        // Lerp para el Zoom
        if (std::abs(diffZoom) > 0.005f) // Si la diferencia de zoom es significativa
        {
            zoomActual += diffZoom * velocidadTransicion * dt;
            float escalaVisual = zoomActual / 5.f;
            sf::Vector2f winSize(window.getSize());
            vista.setSize({winSize.x * escalaVisual, winSize.y * escalaVisual});
            vistaCambiada = true;
        }
        else if (std::abs(diffZoom) > 0.0f)
        {
            // Diferencia insignificante: Forzar el zoom final
            zoomActual = zoomObjetivo;
            float escalaVisual = zoomActual / 5.f;
            sf::Vector2f winSize(window.getSize());
            vista.setSize({winSize.x * escalaVisual, winSize.y * escalaVisual});
            vistaCambiada = true;
        }

        // Aplicar a la camara solo si hubo movimiento
        if (vistaCambiada) 
        {
            window.setView(vista);
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
    vertice nodoSeleccionado = obtenerVerticePorClic(cabeza, posicionMouse, 8.f);

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

void manejarZoomRaton(sf::RenderWindow &window, sf::View &vista, const sf::Event::MouseWheelScrolled *rueda, float &zoomActual, const sf::Vector2u &textureSize)
{
    float factorZoom = 1.f - rueda->delta * 0.1f;
    float nuevoZoom = zoomActual * factorZoom;

    // Calcular el limite dinamico
    sf::Vector2f winSize(window.getSize());
    float maxZoomX = (static_cast<float>(textureSize.x) * 5.f) / winSize.x;
    float maxZoomY = (static_cast<float>(textureSize.y) * 5.f) / winSize.y;
    
    // El zoom maximo sera 5.f, a menos que el mapa exija un limite menor para no ver el fondo
    float maxZoomPermitido = std::min({5.f, maxZoomX, maxZoomY});
    // Garantizar que el maximo no colapse por el minimo 2.5f
    maxZoomPermitido = std::max(2.5f, maxZoomPermitido); 

    // Aplicar los limites al zoom
    if (nuevoZoom < 2.5f) 
    {
        factorZoom = 2.5f / zoomActual;
        zoomActual = 2.5f;
    } 
    else if (nuevoZoom > maxZoomPermitido) 
    {
        // Si nos pasamos hacia arriba, calculamos el factor justo para llegar a 5.f
        factorZoom = maxZoomPermitido / zoomActual;
        zoomActual = maxZoomPermitido;
    }
    else
    {
        zoomActual = nuevoZoom;
    }

    // Aplicar transformaciones solo si el zoom realmente va a cambiar
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

        // Bloquear bordes para que la camara  se deslice
        sf::Vector2f centro = vista.getCenter();
        sf::Vector2f viewSize = vista.getSize();

        float minX = viewSize.x / 2.f;
        float maxX = static_cast<float>(textureSize.x) - minX;
        float minY = viewSize.y / 2.f;
        float maxY = static_cast<float>(textureSize.y) - minY;

        // Si la vista es mas grande, anclar al centro del mapa
        // Si es más pequeña (Zoom 2.5f), aplicar el clamp normal para chocar con los bordes.
        if (minX > maxX) centro.x = static_cast<float>(textureSize.x) / 2.f;
        else centro.x = std::clamp(centro.x, minX, maxX);

        if (minY > maxY) centro.y = static_cast<float>(textureSize.y) / 2.f;
        else centro.y = std::clamp(centro.y, minY, maxY);

        vista.setCenter(centro);
        
        // Aplicar la vista actualizada
        window.setView(vista);
    }
}

void guardarRutaValida(EstadoPathfinder &estado)
{
    if (estado.mostrarRuta && !estado.rutaDijkstra.empty()) 
    {
        std::cout << "[S] Guardando la ruta actual...\n";

        // Crear un archivo temporal con los IDs de la ruta
        std::ofstream archivoTemporal("../assets/ruta_temp.txt");
        for (vertice v : estado.rutaDijkstra) archivoTemporal << v->id << ",";

        archivoTemporal.close();

        // Comprimir la ruta con Huffman
        comprimirDatosMapa("../assets/ruta_temp.txt", "../assets/ruta_guardada.bin");

        std::cout << "Ruta comprimida y guardada exitosamente\n";
    }

    else 
    {
        std::cout << "No hay ninguna ruta en pantalla para guardar\n";
    }
}

void cargarRutaGuardada(EstadoPathfinder &estado, vertice cabeza)
{
    std::cout << "[L] Cargando ultima ruta guardada...\n";

    // Descomprimir el binario
    descomprimirDatosMapa("../assets/ruta_guardada.bin", "../assets/ruta_temp.txt");

    // Leer los IDs y reconstruir el vector rutaDijkstra
    std::ifstream archivoTemporal("../assets/ruta_temp.txt");
    std::string linea;
    if (std::getline(archivoTemporal, linea)) 
    {
        estado.rutaDijkstra.clear();
        std::string id = "";

        for (char c : linea)
        {
            if (c == ',')
            {
                if (!id.empty())
                {
                    int idBuscado = std::stoi(id);
                    // Buscar el nodo en el grafo general y agregarlo a la ruta
                    vertice nodoEncontrado = buscarVertice(cabeza, idBuscado);
                    if (nodoEncontrado) estado.rutaDijkstra.push_back(nodoEncontrado);
                    id = "";
                }
            }
            
            else id += c;
        }

        // Actualizar la interfaz para que SFML dibuje las líneas
        if (!estado.rutaDijkstra.empty())
        {
            estado.origen = estado.rutaDijkstra.front();
            estado.destino = estado.rutaDijkstra.back();
            estado.mostrarRuta = true;
        }
    }
    archivoTemporal.close();
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

std::vector<sf::Vector2f> generarPuntosSuavizados(const std::vector<sf::Vector2f> &puntos, int segmentosPorTramo = 8)
{
    if (puntos.size() < 3) return puntos;

    // Catmull-Rom necesita un punto "fantasma" antes del primero y despues
    // del ultimo para poder calcular las tangentes de los extremos
    std::vector<sf::Vector2f> extendidos;
    extendidos.push_back(puntos.front());
    extendidos.insert(extendidos.end(), puntos.begin(), puntos.end());
    extendidos.push_back(puntos.back());

    std::vector<sf::Vector2f> suavizados;

    for (size_t i = 0; i + 3 < extendidos.size(); ++i)
    {
        sf::Vector2f p0 = extendidos[i];
        sf::Vector2f p1 = extendidos[i + 1];
        sf::Vector2f p2 = extendidos[i + 2];
        sf::Vector2f p3 = extendidos[i + 3];

        for (int j = 0; j < segmentosPorTramo; ++j)
        {
            float t = static_cast<float>(j) / static_cast<float>(segmentosPorTramo);
            float t2 = t * t;
            float t3 = t2 * t;

            float x = 0.5f * ((2.f * p1.x) + (-p0.x + p2.x) * t +
                (2.f * p0.x - 5.f * p1.x + 4.f * p2.x - p3.x) * t2 +
                (-p0.x + 3.f * p1.x - 3.f * p2.x + p3.x) * t3);

            float y = 0.5f * ((2.f * p1.y) + (-p0.y + p2.y) * t +
                (2.f * p0.y - 5.f * p1.y + 4.f * p2.y - p3.y) * t2 +
                (-p0.y + 3.f * p1.y - 3.f * p2.y + p3.y) * t3);

            suavizados.push_back(sf::Vector2f(x, y));
        }
    }

    suavizados.push_back(puntos.back());

    return suavizados;
}

void dibujarRutaDijkstra(sf::RenderWindow &window, const std::vector<vertice> &ruta)
{
    float grosor = 3.f;

    if (ruta.size() < 2) return;

    // Construir la lista de puntos crudos siguiendo el camino REAL de cada
    // tramo de la ruta: buscamos la arista real entre cada par de nodos
    // consecutivos y usamos sus waypoints trazados a mano en vez de una
    // linea recta.
    std::vector<sf::Vector2f> puntosCrudos;
    puntosCrudos.push_back(sf::Vector2f(ruta[0]->coordenadasX, ruta[0]->coordenadasY));

    for (size_t i = 0; i + 1 < ruta.size(); ++i)
    {
        arista tramo = buscarArista(ruta[i], ruta[i + 1]);

        // Si la arista existe y tiene waypoints registrados, los agregamos
        // en orden (ya vienen orientados en el sentido origen->destino que
        // corresponde a este tramo, gracias a enlaceBidireccional)
        if (tramo != nullptr)
        {
            for (const sf::Vector2f &punto : tramo->puntosIntermedios)
                puntosCrudos.push_back(punto);
        }

        // Cerrar el tramo en el nodo destino. Si no se encontro la arista o
        // no tiene waypoints, esto deja una linea recta de respaldo.
        puntosCrudos.push_back(sf::Vector2f(ruta[i + 1]->coordenadasX, ruta[i + 1]->coordenadasY));
    }

    // Aplicar el suavizado Catmull-Rom sobre el trazado real completo
    std::vector<sf::Vector2f> puntos = generarPuntosSuavizados(puntosCrudos);

    if (puntos.empty()) return;

    sf::Color colorRuta = sf::Color::Red;

    // Dibujar los segmentos y las uniones
    for (size_t i = 0; i + 1 < puntos.size(); ++i)
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