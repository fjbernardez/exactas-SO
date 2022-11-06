#ifndef DEFINICIONES_H
#define DEFINICIONES_H

using namespace std;

enum direccion {
    ARRIBA = 0,
    ABAJO,
    IZQUIERDA,
    DERECHA
};

enum color {
    AZUL = 0,
    ROJO,
    INDEFINIDO,
    VACIO,
    BANDERA_ROJA,
    BANDERA_AZUL
};

enum estrategia {
    SECUENCIAL = 0, RR, SHORTEST, USTEDES
};

typedef pair<int, int> coordenadas;

#endif /* DEFINICIONES_H */
