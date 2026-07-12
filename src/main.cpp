#include "Dijkstra.hpp"
#include "grafo.hpp"
#include "interfaz.hpp"

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

// ============================================================
// ESTRUCTURA PARA ALMACENAR NODOS DIBUJABLES EN ESPACIO MUNDO
// ============================================================

struct NodoGrafico
{
    vertice verticePtr;
    sf::CircleShape forma;
    bool seleccionado;

    NodoGrafico() : verticePtr(nullptr), seleccionado(false) {}

    NodoGrafico(vertice v, float radio, sf::Color color)
        : verticePtr(v), seleccionado(false)
    {
        forma.setRadius(radio);
        forma.setFillColor(color);
        forma.setOutlineColor(sf::Color::White);
        forma.setOutlineThickness(2.0f);
        // Posicionar en coordenadas del mundo (pixeles de la imagen)
        forma.setPosition(sf::Vector2f(v->coordenadasX - radio, v->coordenadasY - radio));
    }
};

// ============================================================
// FUNCION AUXILIAR: cargar una fuente probando varias rutas
// (SFML 3 renombro Font::loadFromFile -> Font::openFromFile)
// ============================================================

bool cargarFuentePredeterminada(sf::Font &fuente)
{
    return fuente.openFromFile("assets/arial.ttf") ||
           fuente.openFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf") ||
           fuente.openFromFile("/System/Library/Fonts/Helvetica.ttc") ||
           fuente.openFromFile("C:/Windows/Fonts/arial.ttf");
}

// ============================================================
// FUNCIONES PARA DIBUJADO EN SFML CON VIEW (ESPACIO MUNDO)
// ============================================================

/**
 * Dibuja el mapa con la ruta superpuesta en la ventana SFML
 * utilizando sf::View para coordenadas del mundo
 */
void dibujarMapaConRuta(sf::RenderWindow &ventana, sf::View &vista,
                        const sf::Texture &texturaMapa,
                        const std::vector<vertice> &ruta, bool rutaValida,
                        vertice origen, vertice destino,
                        const std::vector<NodoGrafico> &nodos,
                        int nodoSeleccionado,
                        sf::Font &fuente, bool fuenteCargada)
{
    // Aplicar la vista
    ventana.setView(vista);

    // Limpiar ventana
    ventana.clear(sf::Color(30, 30, 30));

    // Dibujar el mapa en coordenadas del mundo
    sf::Sprite spriteMapa(texturaMapa);
    spriteMapa.setPosition(sf::Vector2f(0, 0)); // El mapa comienza en (0,0) en el mundo
    ventana.draw(spriteMapa);

    // ----- DIBUJAR TODOS LOS NODOS EN COORDENADAS MUNDO -----
    for (size_t i = 0; i < nodos.size(); i++)
    {
        ventana.draw(nodos[i].forma);

        // Si esta seleccionado, dibujar un resaltado
        if (nodoSeleccionado == static_cast<int>(i))
        {
            sf::CircleShape resaltado;
            resaltado.setRadius(nodos[i].forma.getRadius() + 6.0f);
            resaltado.setFillColor(sf::Color(255, 255, 255, 80));
            resaltado.setOutlineColor(sf::Color::Yellow);
            resaltado.setOutlineThickness(2.0f);
            resaltado.setPosition(nodos[i].forma.getPosition() - sf::Vector2f(6.0f, 6.0f));
            ventana.draw(resaltado);
        }
    }

    if (rutaValida && ruta.size() >= 2)
    {
        // ----- DIBUJAR ARISTAS DE LA RUTA EN COORDENADAS MUNDO -----
        for (size_t i = 0; i < ruta.size() - 1; i++)
        {
            vertice v1 = ruta[i];
            vertice v2 = ruta[i + 1];

            // Usar coordenadas del mundo (pixeles de la imagen)
            float x1 = v1->coordenadasX;
            float y1 = v1->coordenadasY;
            float x2 = v2->coordenadasX;
            float y2 = v2->coordenadasY;

            // Dibujar linea principal
            sf::Vertex line[2];
            line[0].position = sf::Vector2f(x1, y1);
            line[0].color = sf::Color(255, 255, 0, 230);
            line[1].position = sf::Vector2f(x2, y2);
            line[1].color = sf::Color(255, 255, 0, 230);
            ventana.draw(line, 2, sf::PrimitiveType::Lines);

            // Dibujar lineas de contorno para mejor visibilidad
            sf::Vertex outline1[2];
            outline1[0].position = sf::Vector2f(x1 + 1.5f, y1);
            outline1[0].color = sf::Color(0, 0, 0, 150);
            outline1[1].position = sf::Vector2f(x2 + 1.5f, y2);
            outline1[1].color = sf::Color(0, 0, 0, 150);
            ventana.draw(outline1, 2, sf::PrimitiveType::Lines);

            sf::Vertex outline2[2];
            outline2[0].position = sf::Vector2f(x1 - 1.5f, y1);
            outline2[0].color = sf::Color(0, 0, 0, 150);
            outline2[1].position = sf::Vector2f(x2 - 1.5f, y2);
            outline2[1].color = sf::Color(0, 0, 0, 150);
            ventana.draw(outline2, 2, sf::PrimitiveType::Lines);
        }

        // ----- DIBUJAR VERTICES DE LA RUTA DESTACADOS -----
        for (size_t i = 0; i < ruta.size(); i++)
        {
            vertice v = ruta[i];

            for (const auto &nodo : nodos)
            {
                if (nodo.verticePtr == v)
                {
                    sf::CircleShape resaltado;
                    resaltado.setRadius(nodo.forma.getRadius() + 4.0f);

                    if (i == 0) // Origen
                        resaltado.setFillColor(sf::Color(0, 100, 255, 100));
                    else if (i == ruta.size() - 1) // Destino
                        resaltado.setFillColor(sf::Color(0, 220, 0, 100));
                    else // Intermedio
                        resaltado.setFillColor(sf::Color(255, 50, 50, 100));

                    resaltado.setOutlineColor(sf::Color::White);
                    resaltado.setOutlineThickness(2.0f);
                    resaltado.setPosition(nodo.forma.getPosition() - sf::Vector2f(4.0f, 4.0f));
                    ventana.draw(resaltado);
                    break;
                }
            }
        }

        // ----- MOSTRAR INFORMACION DE LA RUTA -----
        int pesoTotal = 0;
        for (size_t i = 0; i < ruta.size() - 1; i++)
        {
            arista actual = ruta[i]->aristaAdyacente;
            while (actual != nullptr)
            {
                if (actual->destino == ruta[i + 1])
                {
                    pesoTotal += actual->peso;
                    break;
                }
                actual = actual->siguiente;
            }
        }

        if (fuenteCargada)
        {
            sf::Vector2f centroVista = vista.getCenter();
            sf::Vector2f tamVista = vista.getSize();

            float xInfo = centroVista.x - tamVista.x / 2.0f + 10.0f;
            float yInfo = centroVista.y + tamVista.y / 2.0f - 70.0f;

            // SFML 3: el constructor de sf::Text es (font, string, characterSize)
            sf::Text txtOrigen(fuente, "ORIGEN: " + origen->nombre, 14);
            txtOrigen.setFillColor(sf::Color(100, 200, 255));
            txtOrigen.setPosition(sf::Vector2f(xInfo, yInfo));
            ventana.draw(txtOrigen);

            sf::Text txtDestino(fuente, "DESTINO: " + destino->nombre, 14);
            txtDestino.setFillColor(sf::Color(100, 255, 100));
            txtDestino.setPosition(sf::Vector2f(xInfo, yInfo + 18.0f));
            ventana.draw(txtDestino);

            sf::Text txtPeso(fuente, "PESO TOTAL: " + std::to_string(pesoTotal) + " m", 14);
            txtPeso.setFillColor(sf::Color::Yellow);
            txtPeso.setPosition(sf::Vector2f(xInfo, yInfo + 36.0f));
            ventana.draw(txtPeso);

            sf::Text txtAyuda(fuente, "ESC:Salir  Rueda:Zoom  Click:Seleccionar  R:Reset", 12);
            txtAyuda.setFillColor(sf::Color(200, 200, 200));
            txtAyuda.setPosition(sf::Vector2f(centroVista.x + tamVista.x / 2.0f - 280.0f,
                                              centroVista.y - tamVista.y / 2.0f + 10.0f));
            ventana.draw(txtAyuda);

            // Mostrar coordenadas del mouse en el mundo - USANDO mapPixelToCoords
            sf::Vector2i mousePixel = sf::Mouse::getPosition(ventana);
            sf::Vector2f mouseMundo = ventana.mapPixelToCoords(mousePixel);

            std::stringstream ss;
            ss << "Mouse: (" << std::fixed << std::setprecision(1)
               << mouseMundo.x << ", " << mouseMundo.y << ")";
            sf::Text txtMouse(fuente, ss.str(), 12);
            txtMouse.setFillColor(sf::Color(200, 200, 200));
            txtMouse.setPosition(sf::Vector2f(centroVista.x - tamVista.x / 2.0f + 10.0f,
                                              centroVista.y - tamVista.y / 2.0f + 30.0f));
            ventana.draw(txtMouse);

            // Mostrar nivel de zoom
            sf::Text txtZoom(fuente, "Zoom: " + std::to_string(1.0f / vista.getSize().x * (tamVista.x)), 12);
            txtZoom.setFillColor(sf::Color(200, 200, 200));
            txtZoom.setPosition(sf::Vector2f(centroVista.x + tamVista.x / 2.0f - 150.0f,
                                             centroVista.y - tamVista.y / 2.0f + 30.0f));
            ventana.draw(txtZoom);
        }
    }
    else
    {
        // ----- NO HAY RUTA DISPONIBLE -----
        if (fuenteCargada)
        {
            sf::Vector2f centroVista = vista.getCenter();

            sf::Text txtError(fuente, "NO HAY RUTA DISPONIBLE", 30);
            txtError.setFillColor(sf::Color(255, 50, 50));
            txtError.setStyle(sf::Text::Style::Bold);

            // SFML 3: sf::FloatRect ya no tiene .width/.height, usa .size.x/.size.y
            sf::FloatRect bounds = txtError.getLocalBounds();
            txtError.setPosition(sf::Vector2f(centroVista.x - bounds.size.x / 2.0f,
                                              centroVista.y - bounds.size.y / 2.0f));
            ventana.draw(txtError);

            sf::Text txtAyuda(fuente, "Presiona ESC para cerrar", 14);
            txtAyuda.setFillColor(sf::Color(200, 200, 200));
            sf::FloatRect boundsAyuda = txtAyuda.getLocalBounds();
            txtAyuda.setPosition(sf::Vector2f(centroVista.x - boundsAyuda.size.x / 2.0f,
                                              centroVista.y + 30.0f));
            ventana.draw(txtAyuda);
        }
    }

    ventana.display();
}

// ============================================================
// FUNCION PARA CREAR NODOS GRAFICOS
// ============================================================

std::vector<NodoGrafico> crearNodosGraficos(const std::vector<vertice> &vertices)
{
    std::vector<NodoGrafico> nodos;

    for (vertice v : vertices)
    {
        sf::Color color(180, 180, 200);
        NodoGrafico nodo(v, 8.0f, color);
        nodos.push_back(nodo);
    }

    return nodos;
}

// ============================================================
// FUNCION PARA DETECTAR NODO SELECCIONADO EN ESPACIO MUNDO
// ============================================================

int detectarNodoSeleccionado(const std::vector<NodoGrafico> &nodos,
                             const sf::Vector2f &posicionMundo)
{
    for (size_t i = 0; i < nodos.size(); i++)
    {
        float radio = nodos[i].forma.getRadius();
        sf::Vector2f centro = nodos[i].forma.getPosition() + sf::Vector2f(radio, radio);

        float dx = posicionMundo.x - centro.x;
        float dy = posicionMundo.y - centro.y;
        float distancia = std::sqrt(dx * dx + dy * dy);

        if (distancia <= radio + 5.0f)
        {
            return static_cast<int>(i);
        }
    }

    return -1;
}

// ============================================================
// FUNCION PRINCIPAL DE VISUALIZACION CON VIEW
// ============================================================

// ============================================================
// FUNCION AUXILIAR: ajustar la vista para que un conjunto de
// vertices quede visible dentro de la ventana, sin bajar del
// zoom minimo permitido (1:1)
// ============================================================

void ajustarVistaAVertices(sf::View &vista, const sf::Vector2u &tamVentana,
                           const std::vector<vertice> &verticesAMostrar,
                           float &zoomActual, float margen = 80.0f)
{
    if (verticesAMostrar.empty())
        return;

    float minX = verticesAMostrar[0]->coordenadasX;
    float maxX = verticesAMostrar[0]->coordenadasX;
    float minY = verticesAMostrar[0]->coordenadasY;
    float maxY = verticesAMostrar[0]->coordenadasY;

    for (vertice v : verticesAMostrar)
    {
        minX = std::min(minX, static_cast<float>(v->coordenadasX));
        maxX = std::max(maxX, static_cast<float>(v->coordenadasX));
        minY = std::min(minY, static_cast<float>(v->coordenadasY));
        maxY = std::max(maxY, static_cast<float>(v->coordenadasY));
    }

    float anchoNecesario = (maxX - minX) + margen * 2.0f;
    float altoNecesario = (maxY - minY) + margen * 2.0f;

    float anchoVentana = static_cast<float>(tamVentana.x);
    float altoVentana = static_cast<float>(tamVentana.y);

    // Nunca bajar de zoom 1:1 (no se permite acercar mas de la escala inicial)
    float factorNecesario = std::max({1.0f, anchoNecesario / anchoVentana, altoNecesario / altoVentana});

    vista.setSize(sf::Vector2f(anchoVentana * factorNecesario, altoVentana * factorNecesario));
    vista.setCenter(sf::Vector2f((minX + maxX) / 2.0f, (minY + maxY) / 2.0f));

    zoomActual = factorNecesario;
}

void mostrarRutaEnVentana(const std::vector<vertice> &ruta,
                          vertice origen, vertice destino,
                          const std::vector<vertice> &todosLosVertices)
{
    // Crear ventana
    sf::VideoMode vm;
    vm.size = sf::Vector2u(800, 600);
    sf::RenderWindow ventana(vm, "Optimal PathFinder - UNMSM");

    // Configurar icono
    sf::Image icono;
    if (icono.loadFromFile("assets/logo.png"))
    {
        ventana.setIcon(icono.getSize(), icono.getPixelsPtr());
    }

    // Cargar textura del mapa
    sf::Texture texturaMapa;
    if (!texturaMapa.loadFromFile("assets/mapa.png"))
    {
        std::cerr << "Error: No se pudo cargar assets/mapa.png" << std::endl;
        return;
    }

    // Obtener tamano de la textura (coordenadas del mundo)
    sf::Vector2u tamMapa = texturaMapa.getSize();

    // ============================================================
    // CONFIGURACION DE sf::View - Tamano de la ventana, zoom inicial 1:1
    // ============================================================

    // El ticket pide: inicializar sf::View con el tamano ACTUAL de la
    // ventana, no con el tamano del mapa. Asi las coordenadas de mouse
    // (mapPixelToCoords) coinciden exactamente con los nodos dibujados.
    sf::Vector2u tamVentana = ventana.getSize();
    sf::View vista(sf::FloatRect(sf::Vector2f(0.f, 0.f),
                                 sf::Vector2f(static_cast<float>(tamVentana.x),
                                              static_cast<float>(tamVentana.y))));

    // Zoom inicial = escala 1:1 (el limite maximo de acercamiento permitido).
    // Solo se podra alejar (zoom > 1.0f) desde este punto en adelante.
    float zoomActual = 1.0f;

    // Centrar la vista sobre la ruta (o sobre todos los vertices si no hay
    // ruta) para que quede visible desde el inicio. Si todo cabe a 1:1,
    // el zoom se queda en 1.0f; si no cabe, se aleja lo minimo necesario.
    if (!ruta.empty())
        ajustarVistaAVertices(vista, tamVentana, ruta, zoomActual);
    else
        ajustarVistaAVertices(vista, tamVentana, todosLosVertices, zoomActual);

    // Crear nodos graficos en coordenadas mundo
    std::vector<NodoGrafico> nodos = crearNodosGraficos(todosLosVertices);

    // Colorear nodos de la ruta
    bool rutaValida = !ruta.empty();
    if (rutaValida)
    {
        for (size_t i = 0; i < ruta.size(); i++)
        {
            for (auto &nodo : nodos)
            {
                if (nodo.verticePtr == ruta[i])
                {
                    if (i == 0)
                        nodo.forma.setFillColor(sf::Color(0, 100, 255));
                    else if (i == ruta.size() - 1)
                        nodo.forma.setFillColor(sf::Color(0, 220, 0));
                    else
                        nodo.forma.setFillColor(sf::Color(255, 50, 50));
                    break;
                }
            }
        }
    }

    // Cargar la fuente UNA SOLA VEZ, antes del bucle (no en cada frame)
    sf::Font fuente;
    bool fuenteCargada = cargarFuentePredeterminada(fuente);
    if (!fuenteCargada)
    {
        std::cerr << "Aviso: no se encontro ninguna fuente. El texto no se mostrara." << std::endl;
    }

    // Variables de control para la camara
    bool arrastrando = false;
    sf::Vector2f posInicioArrastre;
    sf::Vector2f centroInicio;
    int nodoSeleccionado = -1;

    // Bucle principal
    while (ventana.isOpen())
    {
        while (const std::optional event = ventana.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                ventana.close();
            }

            if (const auto *tecla = event->getIf<sf::Event::KeyPressed>())
            {
                if (tecla->scancode == sf::Keyboard::Scancode::Escape)
                {
                    ventana.close();
                }
                if (tecla->scancode == sf::Keyboard::Scancode::R)
                {
                    // Resetear vista: mismo encuadre inicial centrado en la ruta
                    vista = sf::View(sf::FloatRect(sf::Vector2f(0.f, 0.f),
                                                   sf::Vector2f(static_cast<float>(tamVentana.x),
                                                                static_cast<float>(tamVentana.y))));
                    zoomActual = 1.0f;
                    if (!ruta.empty())
                        ajustarVistaAVertices(vista, tamVentana, ruta, zoomActual);
                    else
                        ajustarVistaAVertices(vista, tamVentana, todosLosVertices, zoomActual);
                }
            }

            // ============================================================
            // ARRASTRE DE LA CAMARA (Boton derecho del mouse)
            // ============================================================

            if (const auto *boton = event->getIf<sf::Event::MouseButtonPressed>())
            {
                // Boton central o derecho para arrastrar (como pide el ticket)
                if (boton->button == sf::Mouse::Button::Middle ||
                    boton->button == sf::Mouse::Button::Right)
                {
                    arrastrando = true;
                    posInicioArrastre = sf::Vector2f(static_cast<float>(boton->position.x),
                                                     static_cast<float>(boton->position.y));
                    centroInicio = vista.getCenter();
                }

                // Click izquierdo para seleccionar nodos
                if (boton->button == sf::Mouse::Button::Left)
                {
                    // CONVERSION DE COORDENADAS: PIXEL DE PANTALLA -> COORDENADA MUNDO
                    sf::Vector2f posMundo = ventana.mapPixelToCoords(
                        sf::Vector2i(boton->position.x, boton->position.y));

                    nodoSeleccionado = detectarNodoSeleccionado(nodos, posMundo);

                    if (nodoSeleccionado != -1)
                    {
                        std::cout << "\nNodo seleccionado: "
                                  << nodos[nodoSeleccionado].verticePtr->nombre
                                  << " (ID: " << nodos[nodoSeleccionado].verticePtr->id << ")"
                                  << std::endl;
                        std::cout << "  Coordenadas mundo: ("
                                  << nodos[nodoSeleccionado].verticePtr->coordenadasX
                                  << ", "
                                  << nodos[nodoSeleccionado].verticePtr->coordenadasY
                                  << ")" << std::endl;

                        mostrarVertices(nodos[nodoSeleccionado].verticePtr);
                    }
                }
            }

            if (const auto *boton = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (boton->button == sf::Mouse::Button::Middle ||
                    boton->button == sf::Mouse::Button::Right)
                {
                    arrastrando = false;
                }
            }

            if (const auto *movimiento = event->getIf<sf::Event::MouseMoved>())
            {
                if (arrastrando)
                {
                    // Calcular diferencia de posicion fisica
                    sf::Vector2f posActual(static_cast<float>(movimiento->position.x),
                                           static_cast<float>(movimiento->position.y));
                    sf::Vector2f delta = posActual - posInicioArrastre;

                    // Desplazar la camara usando view.move(-delta)
                    vista.move(-delta);

                    // Actualizar posicion de inicio para el proximo movimiento
                    posInicioArrastre = posActual;
                }
            }

            // ============================================================
            // ZOOM CON LA RUEDA DEL MOUSE (SOLO PERMITIR ALEJAR)
            // ============================================================

            if (const auto *rueda = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if (rueda->wheel == sf::Mouse::Wheel::Vertical)
                {
                    // Calcular factor de zoom
                    // delta positivo = scroll hacia arriba (acercar)
                    // delta negativo = scroll hacia abajo (alejar)
                    float factorZoom = 1.0f - rueda->delta * 0.1f;

                    // SOLO PERMITIR ALEJAR (zoom > 1.0f)
                    // Si el zoom actual es 1:1 (minimo), bloquear acercamiento
                    if (factorZoom < 1.0f && zoomActual <= 1.0f)
                    {
                        // Bloquear acercamiento cuando ya estamos en zoom 1:1
                        factorZoom = 1.0f;
                    }

                    // Limitar el zoom maximo (alejamiento) a 5x
                    if (factorZoom > 5.0f)
                        factorZoom = 5.0f;
                    if (factorZoom < 0.5f)
                        factorZoom = 0.5f;

                    // Verificar que no se intente acercar mas alla de 1:1
                    float nuevoZoom = zoomActual * factorZoom;
                    if (nuevoZoom < 1.0f)
                    {
                        // Si el nuevo zoom seria menor que 1:1, ajustar para mantener 1:1
                        factorZoom = 1.0f / zoomActual;
                    }

                    // Obtener posicion del mouse en coordenadas mundo para zoom centrado
                    sf::Vector2f posMouse = ventana.mapPixelToCoords(
                        sf::Vector2i(rueda->position.x, rueda->position.y));

                    // Aplicar zoom
                    vista.zoom(factorZoom);
                    vista.setCenter(posMouse);

                    // Actualizar zoom actual
                    zoomActual *= factorZoom;
                    if (zoomActual < 1.0f)
                        zoomActual = 1.0f;
                }
            }
        }

        dibujarMapaConRuta(ventana, vista, texturaMapa, ruta, rutaValida,
                           origen, destino, nodos, nodoSeleccionado,
                           fuente, fuenteCargada);
    }
}

// ============================================================
// FUNCION PARA IMPRIMIR RUTA EN CONSOLA
// ============================================================

void imprimirRutaConsola(const std::vector<vertice> &ruta)
{
    if (ruta.empty())
    {
        std::cout << "\n========================================" << std::endl;
        std::cout << "NO EXISTE RUTA VALIDA" << std::endl;
        std::cout << "========================================\n"
                  << std::endl;
        return;
    }

    int pesoTotal = 0;

    std::cout << "\n========================================" << std::endl;
    std::cout << "RUTA ENCONTRADA:" << std::endl;
    std::cout << "========================================" << std::endl;

    for (size_t i = 0; i < ruta.size(); i++)
    {
        std::cout << "  " << (i + 1) << ". " << ruta[i]->nombre;

        if (i != ruta.size() - 1)
        {
            arista actual = ruta[i]->aristaAdyacente;
            while (actual != nullptr)
            {
                if (actual->destino == ruta[i + 1])
                {
                    pesoTotal += actual->peso;
                    std::cout << "  ---(" << actual->peso << "m)--->";
                    break;
                }
                actual = actual->siguiente;
            }
        }
        std::cout << std::endl;
    }

    std::cout << "\n----------------------------------------" << std::endl;
    std::cout << "PESO TOTAL: " << pesoTotal << " metros" << std::endl;
    std::cout << "CANTIDAD DE VERTICES: " << ruta.size() << std::endl;
    std::cout << "========================================\n"
              << std::endl;
}

// ============================================================
// MAIN - FLUJO PRINCIPAL DEL PROGRAMA
// ============================================================

int main()
{
    std::cout << "\n";
    std::cout << "======================================================" << std::endl;
    std::cout << "     OPTIMAL PATHFINDER - CIUDAD UNIVERSITARIA" << std::endl;
    std::cout << "              UNIVERSIDAD NACIONAL MAYOR" << std::endl;
    std::cout << "               DE SAN MARCOS - UNMSM" << std::endl;
    std::cout << "======================================================" << std::endl;
    std::cout << std::endl;

    // ============================================================
    // PASO 1: CARGAR EL GRAFO DESDE EL ARCHIVO
    // ============================================================

    std::cout << "[1] Cargando grafo desde assets/mapa.txt..." << std::endl;

    vertice cabeza = nullptr;
    cargarGrafoDesdeArchivo(cabeza, "assets/mapa.txt");

    if (!cabeza)
    {
        std::cerr << "\nERROR: No se pudo cargar el grafo desde el archivo." << std::endl;
        std::cerr << "Verifica que el archivo assets/mapa.txt exista." << std::endl;
        return 1;
    }

    std::vector<vertice> listaVertices;
    vertice actual = cabeza;
    while (actual != nullptr)
    {
        listaVertices.push_back(actual);
        actual = actual->siguiente;
    }

    const int TOTAL_VERTICES = static_cast<int>(listaVertices.size());
    std::cout << "   -> Grafo cargado exitosamente. " << TOTAL_VERTICES << " vertices encontrados." << std::endl;
    std::cout << std::endl;

    // ============================================================
    // PASO 2: SOLICITAR DATOS POR CONSOLA
    // ============================================================

    std::cout << "[2] Seleccione origen y destino:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    for (int i = 0; i < TOTAL_VERTICES; i++)
    {
        std::cout << "  " << (i + 1) << ". " << listaVertices[i]->nombre;
        std::cout << "  (" << listaVertices[i]->coordenadasX << ", "
                  << listaVertices[i]->coordenadasY << ")" << std::endl;
    }
    std::cout << "----------------------------------------" << std::endl;

    int indiceOrigen = 0;
    do
    {
        std::cout << "Ingrese numero del vertice ORIGEN: ";
        std::cin >> indiceOrigen;

        if (indiceOrigen < 1 || indiceOrigen > TOTAL_VERTICES)
        {
            std::cout << "  Numero invalido. Intente nuevamente." << std::endl;
        }
    } while (indiceOrigen < 1 || indiceOrigen > TOTAL_VERTICES);

    int indiceDestino = 0;
    do
    {
        std::cout << "Ingrese numero del vertice DESTINO: ";
        std::cin >> indiceDestino;

        if (indiceDestino < 1 || indiceDestino > TOTAL_VERTICES)
        {
            std::cout << "  Numero invalido. Intente nuevamente." << std::endl;
        }
        else if (indiceDestino == indiceOrigen)
        {
            std::cout << "  Origen y destino no pueden ser iguales." << std::endl;
            indiceDestino = 0;
        }
    } while (indiceDestino < 1 || indiceDestino > TOTAL_VERTICES);

    char respuesta;
    std::cout << "¿Filtrar solo rutas accesibles para sillas de ruedas? (s/n): ";
    std::cin >> respuesta;
    bool filtrarAccesibilidad = (respuesta == 's' || respuesta == 'S');

    vertice origen = listaVertices[indiceOrigen - 1];
    vertice destino = listaVertices[indiceDestino - 1];

    std::cout << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "ORIGEN: " << origen->nombre << std::endl;
    std::cout << "DESTINO: " << destino->nombre << std::endl;
    std::cout << "FILTRO ACCESIBLE: " << (filtrarAccesibilidad ? "SI" : "NO") << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << std::endl;

    // ============================================================
    // PASO 3: CALCULAR RUTA CON DIJKSTRA
    // ============================================================

    std::cout << "[3] Calculando ruta mas corta con algoritmo de Dijkstra..." << std::endl;
    std::cout << "   -> Procesando... (las aristas con nivel_restriccion > 0 son ignoradas)" << std::endl;

    std::vector<vertice> ruta = calcularRutaDijkstra(
        cabeza,
        origen,
        destino,
        filtrarAccesibilidad);

    std::cout << "   -> Calculo completado." << std::endl;
    std::cout << std::endl;

    // ============================================================
    // PASO 4: MOSTRAR RESULTADO EN CONSOLA Y VENTANA SFML
    // ============================================================

    imprimirRutaConsola(ruta);

    std::cout << "[4] Abriendo ventana grafica con el mapa..." << std::endl;
    std::cout << "   -> Controles:" << std::endl;
    std::cout << "      - Click izquierdo: Seleccionar nodo" << std::endl;
    std::cout << "      - Click derecho: Arrastrar mapa" << std::endl;
    std::cout << "      - Rueda del mouse: Zoom (solo alejar)" << std::endl;
    std::cout << "      - R: Resetear vista" << std::endl;
    std::cout << "      - ESC: Cerrar ventana" << std::endl;
    std::cout << std::endl;

    mostrarRutaEnVentana(ruta, origen, destino, listaVertices);

    // ============================================================
    // LIMPIEZA Y SALIDA
    // ============================================================

    liberarGrafo(cabeza);

    std::cout << "Programa finalizado correctamente." << std::endl;

    return 0;
}