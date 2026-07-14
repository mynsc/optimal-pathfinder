#include "grafo.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

vertice crearVertice(int id, const std::string &nombre, float coordenadasX, float coordenadasY)
{
    vertice v = new Vertice();

    v->id = id;
    v->nombre = nombre;
    v->coordenadasX = coordenadasX;
    v->coordenadasY = coordenadasY;

    v->siguiente = nullptr;
    v->anterior = nullptr;

    v->aristaAdyacente = nullptr;

    return v;
}

void agregarVertice(vertice &cabeza, vertice v)
{
    if (!cabeza) { cabeza = v; return; }

    vertice actual = cabeza;

    while (actual->siguiente) actual = actual->siguiente;

    actual->siguiente = v;
    v->siguiente = nullptr;
    v->anterior = actual;
}

vertice buscarVertice(vertice cabeza, int id)
{
    vertice actual = cabeza;

    while (actual)
    {
        if (actual->id == id) return actual;
        actual = actual->siguiente;
    }

    return nullptr;
}

void mostrarVertices(vertice origen)
{
    std::cout << "\nOrigen: " << origen->nombre << std::endl;

    arista auxiliar = origen->aristaAdyacente;

    while (auxiliar != nullptr)
    {
        std::cout << origen->nombre;
        std::cout << "  ---(" << auxiliar->peso << ")---> " << auxiliar->destino->nombre << std::endl;
        
        auxiliar = auxiliar->siguiente;
    }
}

arista crearArista(vertice origen, vertice destino, short peso, bool esAccesible, short nivelRestriccion,
                   const std::vector<sf::Vector2f> &puntosIntermedios)
{
    arista a = new Arista();

    a->peso = peso;
    a->esAccesible = esAccesible;
    a->nivelRestriccion = nivelRestriccion;
    a->origen = origen;
    a->destino = destino;
    a->puntosIntermedios = puntosIntermedios;
    a->siguiente = nullptr;
    a->anterior = nullptr;

    return a;
}

void agregarArista(vertice origen, arista arista)
{
    if (!origen) return;

    arista->siguiente = origen->aristaAdyacente;

    if (origen->aristaAdyacente)
        origen->aristaAdyacente->anterior = arista;
    origen->aristaAdyacente = arista;
}

void enlaceBidireccional(vertice a, vertice b, short peso, bool esAccesible, short nivelRestriccion,
                         const std::vector<sf::Vector2f> &puntosIntermedios)
{
    if (!a || !b) return;

    // Los puntosIntermedios recibidos van en sentido a -> b; para b -> a se
    // invierte el orden, asi la ruta se dibuja bien sin importar la
    // direccion en la que Dijkstra recorra la arista.
    std::vector<sf::Vector2f> puntosIntermediosInvertidos(puntosIntermedios.rbegin(), puntosIntermedios.rend());

    arista ab = crearArista(a, b, peso, esAccesible, nivelRestriccion, puntosIntermedios);
    arista ba = crearArista(b, a, peso, esAccesible, nivelRestriccion, puntosIntermediosInvertidos);

    agregarArista(a, ab);
    agregarArista(b, ba);
}

arista buscarArista(vertice origen, vertice destino)
{
    if (!origen || !destino) return nullptr;

    arista actual = origen->aristaAdyacente;

    while (actual)
    {
        if (actual->destino == destino) return actual;
        actual = actual->siguiente;
    }

    return nullptr;
}

void cargarGrafoDesdeArchivo(vertice &cabeza, const std::string &nombreArchivo)
{
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open())
    {
        std::cout << "Error al abrir el archivo: " << nombreArchivo << std::endl;
        return;
    }

    std::string linea;
    std::string seccionActual = "";

    while (std::getline(archivo, linea))
    {
        // Limpiar retornos de carro de Windows (\r) o lineas vacias
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (linea.empty()) continue;

        // Detectar secciones analizando los comentarios
        if (linea[0] == '#')
        {
            if (linea.find("VERTICES") != std::string::npos) seccionActual = "VERTICES";
            if (linea.find("ARISTAS") != std::string::npos) seccionActual = "ARISTAS";
            continue;
        }

        std::stringstream ss(linea);
        std::string token;

        if (seccionActual == "VERTICES")
        {
            int id;
            std::string nombre;
            float coordenadasX, coordenadasY;

            std::getline(ss, token, '|'); id = std::stoi(token);
            std::getline(ss, nombre, '|');
            std::getline(ss, token, '|'); coordenadasX = std::stof(token);
            std::getline(ss, token, '|'); coordenadasY = std::stof(token);

            // Crear el vertice con todos los datos del archivo
            vertice nuevo = crearVertice(id, nombre, coordenadasX, coordenadasY);
            agregarVertice(cabeza, nuevo);
        } 
        else if (seccionActual == "ARISTAS")
        {
            int idOrigen, idDestino;
            short peso, nivelRestriccion;
            std::string stringAccesible;
            std::string stringWaypoints;

            std::getline(ss, token, '|'); idOrigen = std::stoi(token);
            std::getline(ss, token, '|'); idDestino = std::stoi(token);
            std::getline(ss, token, '|'); peso = static_cast<short>(std::stoi(token));
            std::getline(ss, stringAccesible, '|');
            std::getline(ss, token, '|'); nivelRestriccion = static_cast<short>(std::stoi(token));

            std::vector<sf::Vector2f> puntosIntermedios;
            if (std::getline(ss, stringWaypoints, '|') && !stringWaypoints.empty())
            {
                std::stringstream ssPuntos(stringWaypoints);
                std::string puntoStr;

                while (std::getline(ssPuntos, puntoStr, ';'))
                {
                    if (puntoStr.empty()) continue;

                    std::stringstream ssCoord(puntoStr);
                    std::string coordX, coordY;

                    if (std::getline(ssCoord, coordX, ',') && std::getline(ssCoord, coordY, ','))
                        puntosIntermedios.push_back(sf::Vector2f(std::stof(coordX), std::stof(coordY)));
                }
            }

            // Buscar los punteros reales de los vertices usando el ID numerico
            vertice origen = buscarVertice(cabeza, idOrigen);
            vertice destino = buscarVertice(cabeza, idDestino);

            // Si ambos existen en memoria, los enlazamos
            if (origen != nullptr && destino != nullptr)
            {
                bool esAccesible = (stringAccesible == "true");
                enlaceBidireccional(origen, destino, peso, esAccesible, nivelRestriccion, puntosIntermedios);
            }
        }
    }
    archivo.close();
}

void liberarGrafo(vertice &cabeza)
{
    vertice actualV = cabeza;

    while (actualV) 
    {
        arista actualA = actualV->aristaAdyacente;

        while (actualA)
        {
            arista siguienteA = actualA->siguiente;
            delete actualA;
            actualA = siguienteA;
        }

        vertice siguienteV = actualV->siguiente;
        delete actualV;
        actualV = siguienteV;
    }

    cabeza = nullptr;
}