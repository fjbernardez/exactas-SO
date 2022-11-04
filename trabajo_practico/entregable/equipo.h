#ifndef EQUIPO_H
#define EQUIPO_H

#include <semaphore.h>
#include <vector>
#include <thread>
#include "definiciones.h"
#include "gameMaster.h"

using namespace std;

class Equipo {
private:

    // Atributos Privados
    gameMaster *belcebu;
    color contrario, equipo, bandera_contraria;
    estrategia strat;
    int cant_jugadores, quantum, quantum_restante;
    vector<thread> jugadores;
    int cant_jugadores_que_ya_jugaron = 0;
    vector<coordenadas> posiciones;
    coordenadas pos_bandera_contraria;
    //
    // ...
    //
    int cant_jugadores_listos_para_jugar = 0;
    int cant_jugadores_ya_saben_bandera = 0;
    int buscar_bandera_i = 0;
    mutex equipo_coordinacion_mutex;
    mutex clock_coordinacion_mutex;
    sem_t equipo_coordinacion_sem_salida;
    sem_t equipo_coordinacion_sem_entrada;
    sem_t equipo_coordinacion_sem_bandera;
    vector<sem_t> rr_coordinacion_sem;
    int tam_Y;
    int tam_X;
    bool busqueda_distribuida;
    struct timespec start, finish;

    // Métodos privados
    direccion apuntar_a(coordenadas pos2, coordenadas pos1);

    void jugador(int nro_jugador);

    coordenadas buscar_bandera_contraria(int nro_jugador);
    //
    // ...
    //
    void jugar_turno_estrategia_secuencial(int nro_jugador);
    void jugar_turno_estrategia_rr(int nro_jugador);
    void jugar_turno_estrategia_shortest(int nro_jugador);
    void jugar_turno_estrategia_ustedes(int nro_jugador);
public:
    Equipo(gameMaster *belcebu, color equipo,
           estrategia strat, int cant_jugadores, int quantum, vector<coordenadas> posiciones, bool busqueda_distribuida);

    void comenzar();

    void terminar();
    // crear jugadores

};

#endif // EQUIPO_H
