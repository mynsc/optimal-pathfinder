#pragma once
#include <SFML/System/Vector2.hpp>

#include <string>
#include <vector>

struct Vertice;
struct Arista;

struct Vertice{
    int id = 0;
    std::string nombre = "";
    float coordenadasX = 0.0;
    float coordenadasY = 0.0;

    Vertice *siguiente = nullptr;
    Vertice *anterior = nullptr;

    Arista *aristaAdyacente = nullptr;
};

struct Arista
{
    short peso = 0;
    bool esAccesible = true;
    short nivelRestriccion = 0;
    
    Vertice *origen = nullptr;
    Vertice *destino = nullptr;

    // Pixeles intermedios que trazan el camino real
    std::vector<sf::Vector2f> puntosIntermedios;

    Arista *siguiente = nullptr;
    Arista *anterior = nullptr;
};

typedef Arista *arista;
typedef Vertice *vertice;

vertice crearVertice(int id, const std::string &nombre, float coordenadasX, float coordenadasY);
void agregarVertice(vertice &cabeza, vertice v);
vertice buscarVertice(vertice cabeza, int id);
void mostrarVertices(vertice vertice);

arista crearArista(vertice origen, vertice destino, short peso, bool esAccesible, short niveRestriccion,
                   const std::vector<sf::Vector2f> &waypoints = {});
void agregarArista(vertice origen, arista arista);
void enlaceBidireccional(vertice a, vertice b, short peso, bool esAccesible, short nivelRestriccion,
                         const std::vector<sf::Vector2f> &waypoints = {});
arista buscarArista(vertice origen, vertice destino);

void cargarGrafoDesdeArchivo(vertice &cabeza, const std::string &nombre_archivo);
void liberarGrafo(vertice &cabeza);