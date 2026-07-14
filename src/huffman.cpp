#include "huffman.hpp"

#include <bitset>
#include <fstream>
#include <iostream>
#include <queue>

// Estructura para que la cola de prioridad ordene de menor a mayor frecuencia
struct CompararNodos {
    bool operator()(NodoHuffman* izquierdo, NodoHuffman* derecho) {

        // Si las frecuencias son iguales, el empate se rompe por el caracter
        if (izquierdo->frecuencia == derecho->frecuencia)
        {
            return izquierdo->caracter > derecho->caracter;
        }

        return izquierdo->frecuencia > derecho->frecuencia;
    }
};

// Funcion recursiva para generar los codigos binarios variables
void generarCodigos(NodoHuffman* raiz, std::string codigoActual, std::unordered_map<char, std::string>& codigos) {
    if (!raiz) return;
    // Si es una hoja, guardar el codigo generado
    if (!raiz->izquierdo && !raiz->derecho) {
        codigos[raiz->caracter] = codigoActual;
    }
    generarCodigos(raiz->izquierdo, codigoActual + "0", codigos);
    generarCodigos(raiz->derecho, codigoActual + "1", codigos);
}

void liberarArbolHuffman(NodoHuffman* raiz) {
    if (!raiz) return;
    liberarArbolHuffman(raiz->izquierdo);
    liberarArbolHuffman(raiz->derecho);
    delete raiz;
}

void comprimirDatosMapa(const std::string& rutaEntrada, const std::string& rutaSalida) {
    std::ifstream archivoEntrada(rutaEntrada);
    if (!archivoEntrada.is_open()) return;

    // Leer todo el contenido del archivo plano
    std::string contenido((std::istreambuf_iterator<char>(archivoEntrada)), std::istreambuf_iterator<char>());
    archivoEntrada.close();

    // Analizar frecuencias
    std::unordered_map<char, int> frecuencias;
    for (char c : contenido) frecuencias[c]++;

    // Crear la cola de prioridad
    std::priority_queue<NodoHuffman*, std::vector<NodoHuffman*>, CompararNodos> colaPrioridad;
    for (auto par : frecuencias) {
        // Inicializacion agregada sin constructor
        colaPrioridad.push(new NodoHuffman{par.first, par.second});
    }

    // Construir el arbol de Huffman dinamico
    while (colaPrioridad.size() > 1) {
        NodoHuffman* izquierdo = colaPrioridad.top(); colaPrioridad.pop();
        NodoHuffman* derecho = colaPrioridad.top(); colaPrioridad.pop();

        NodoHuffman* nuevoNodo = new NodoHuffman{'\0', izquierdo->frecuencia + derecho->frecuencia, izquierdo, derecho};
        colaPrioridad.push(nuevoNodo);
    }
    NodoHuffman* raiz = colaPrioridad.top();

    // Asignar codigos
    std::unordered_map<char, std::string> codigos;
    generarCodigos(raiz, "", codigos);

    // Generar el flujo de bits comprimido
    std::string datosComprimidos = "";
    for (char c : contenido) datosComprimidos += codigos[c];

    // Rellenar con ceros al final para completar el ultimo byte
    int bitsExtra = 8 - (datosComprimidos.length() % 8);
    if (bitsExtra == 8) bitsExtra = 0;
    for (int i = 0; i < bitsExtra; ++i) datosComprimidos += "0";

    // Empaquetar todo en un unico archivo binario estructurado
    std::ofstream salidaBinaria(rutaSalida, std::ios::binary);
    
    // Escribir la tabla de frecuencias para la futura descompresion
    int sizeMap = frecuencias.size();
    salidaBinaria.write(reinterpret_cast<const char*>(&sizeMap), sizeof(sizeMap));
    for (auto par : frecuencias) {
        salidaBinaria.write(&par.first, sizeof(par.first));
        salidaBinaria.write(reinterpret_cast<const char*>(&par.second), sizeof(par.second));
    }

    // Escribir los bits extra que seran ignorados en la lectura
    salidaBinaria.write(reinterpret_cast<const char*>(&bitsExtra), sizeof(bitsExtra));

    // Escribir los bits comprimidos byte a byte
    for (int i = 0; i < datosComprimidos.length(); i += 8) {
        std::bitset<8> bits(datosComprimidos.substr(i, 8));
        unsigned char byte = static_cast<unsigned char>(bits.to_ulong());
        salidaBinaria.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
    }

    salidaBinaria.close();
    liberarArbolHuffman(raiz);
}

void descomprimirDatosMapa(const std::string& rutaEntrada, const std::string& rutaSalida) {
    std::ifstream entradaBinaria(rutaEntrada, std::ios::binary);
    if (!entradaBinaria.is_open()) return;

    // Reconstruir la tabla de frecuencias
    int sizeMap;
    entradaBinaria.read(reinterpret_cast<char*>(&sizeMap), sizeof(sizeMap));

    std::priority_queue<NodoHuffman*, std::vector<NodoHuffman*>, CompararNodos> colaPrioridad;

    for (int i = 0; i < sizeMap; ++i)
    {
        char caracter;
        int frecuencia;
        entradaBinaria.read(&caracter, sizeof(caracter));
        entradaBinaria.read(reinterpret_cast<char*>(&frecuencia), sizeof(frecuencia));
        
        // Insertar directamente garantiza el mismo orden exacto del archivo
        colaPrioridad.push(new NodoHuffman{caracter, frecuencia});
    }

    while (colaPrioridad.size() > 1) {
        NodoHuffman* izquierdo = colaPrioridad.top(); colaPrioridad.pop();
        NodoHuffman* derecho = colaPrioridad.top(); colaPrioridad.pop();

        NodoHuffman* nuevoNodo = new NodoHuffman{'\0', izquierdo->frecuencia + derecho->frecuencia, izquierdo, derecho};
        colaPrioridad.push(nuevoNodo);
    }
    NodoHuffman* raiz = colaPrioridad.top();

    // Leer la informacion del padding
    int bitsExtra;
    entradaBinaria.read(reinterpret_cast<char*>(&bitsExtra), sizeof(bitsExtra));

    // Extraer el flujo de bits reconstruido
    std::string datosComprimidos = "";
    unsigned char byte;
    while (entradaBinaria.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        std::bitset<8> bits(byte);
        datosComprimidos += bits.to_string();
    }

    // Eliminar los bits de relleno del final
    if (bitsExtra > 0 && datosComprimidos.length() >= bitsExtra) {
        datosComprimidos.erase(datosComprimidos.length() - bitsExtra);
    }

    // Navegar por el arbol para decodificar los caracteres
    std::string textoDescomprimido = "";
    NodoHuffman* actual = raiz;
    for (char bit : datosComprimidos) {
        if (bit == '0') actual = actual->izquierdo;
        else actual = actual->derecho;

        if (!actual->izquierdo && !actual->derecho) {
            textoDescomprimido += actual->caracter;
            actual = raiz;
        }
    }

    // Escribir el mapa restaurado a texto plano
    std::ofstream archivoSalida(rutaSalida);
    archivoSalida << textoDescomprimido;
    
    archivoSalida.close();
    entradaBinaria.close();
    liberarArbolHuffman(raiz);
}
