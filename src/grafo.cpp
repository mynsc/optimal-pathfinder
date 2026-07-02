#include "grafo.hpp"
#include <iostream>

ListaVertices crearVertice(const std::string &nombre) {
    ListaVertices v = new Vertice();
    v->nombre = nombre;
    v->siguiente = nullptr;
    v->anterior = nullptr;
    v->AristaAdyacente = nullptr;
    return v;
}

void agregarVertice(ListaVertices &head, ListaVertices v) {
    if (!head) { head = v; return; }
    ListaVertices cur = head;
    while (cur->siguiente) cur = cur->siguiente;
    cur->siguiente = v;
    v->siguiente = nullptr;
    v->anterior = cur;
}

ListaVertices buscarVertice(ListaVertices head, const std::string &nombre) {
    ListaVertices cur = head;
    while (cur) {
        if (cur->nombre == nombre) return cur;
        cur = cur->siguiente;
    }
    return nullptr;
}

ListaAristas crearArista(ListaVertices origen, ListaVertices destino, short peso) {
    ListaAristas a = new Arista();
    a->peso = peso;
    a->origen = origen;
    a->destino = destino;
    a->siguiente = nullptr;
    a->anterior = nullptr;
    return a;
}

void agregarArista(ListaVertices origen, ListaAristas arista) {
    if (!origen) return;
    arista->siguiente = origen->AristaAdyacente;
    if (origen->AristaAdyacente)
        origen->AristaAdyacente->anterior = arista;
    origen->AristaAdyacente = arista;
}

void enlaceBidireccional(ListaVertices a, ListaVertices b, short peso) {
    if (!a || !b) return;
    ListaAristas ab = crearArista(a, b, peso);
    ListaAristas ba = crearArista(b, a, peso);
    agregarArista(a, ab);
    agregarArista(b, ba);
}

void liberarGrafo(ListaVertices &head) {
    ListaVertices curV = head;
    while (curV) {
        ListaAristas curA = curV->AristaAdyacente;
        while (curA) {
            ListaAristas nextA = curA->siguiente;
            delete curA;
            curA = nextA;
        }
        ListaVertices nextV = curV->siguiente;
        delete curV;
        curV = nextV;
    }
    head = nullptr;
}