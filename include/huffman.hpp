#pragma once
#include <string>
#include <unordered_map>

// Arbol binario 
struct NodoHuffman {
    char caracter;
    int frecuencia;
    NodoHuffman* izquierdo = nullptr;
    NodoHuffman* derecho = nullptr;
};

void comprimirDatosMapa(const std::string& rutaEntrada, const std::string& rutaSalida);
void descomprimirDatosMapa(const std::string& rutaEntrada, const std::string& rutaSalida);