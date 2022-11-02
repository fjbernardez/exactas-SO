#include <sys/unistd.h>
#include <assert.h>     /* assert */
#include "gameMaster.h"
#include <mutex>
#include <semaphore.h>

bool gameMaster::es_posicion_valida(coordenadas pos) {
    return (pos.first > 0) && (pos.first < x) && (pos.second > 0) && (pos.second < y);
}

bool gameMaster::es_color_libre(color color_tablero) {
    return color_tablero == VACIO || color_tablero == INDEFINIDO;
}

color gameMaster::en_posicion(coordenadas coord) {
    return tablero[coord.first][coord.second];
}

int gameMaster::getTamx() {
    return x;
}

int gameMaster::getTamy() {
    return y;
}

int gameMaster::distancia(coordenadas c1, coordenadas c2) {
    return abs(c1.first - c2.first) + abs(c1.second - c2.second);
}

gameMaster::gameMaster(Config config,estrategia this_game_strat) {
    assert(config.x > 0);
    assert(config.y > 0); // Tamaño adecuado del tablero

    this->x = config.x;
    this->y = config.y;

    assert((config.bandera_roja.first == 1)); // Bandera roja en la primera columna
    assert(es_posicion_valida(config.bandera_roja)); // Bandera roja en algún lugar razonable

    assert((config.bandera_azul.first == x - 1)); // Bandera azul en la primera columna
    assert(es_posicion_valida(config.bandera_azul)); // Bandera roja en algún lugar razonable

    assert(config.pos_rojo.size() == config.cantidad_jugadores);
    assert(config.pos_azul.size() == config.cantidad_jugadores);
    for (auto &coord: config.pos_rojo) {
        assert(es_posicion_valida(coord)); // Posiciones validas rojas
    }

    for (auto &coord: config.pos_azul) {
        assert(es_posicion_valida(coord)); // Posiciones validas rojas
    }


    this->jugadores_por_equipos = config.cantidad_jugadores;
    this->pos_bandera_roja = config.bandera_roja;
    this->pos_bandera_azul = config.bandera_azul;
    this->pos_jugadores_rojos = config.pos_rojo;
    this->pos_jugadores_azules = config.pos_azul;
    // Seteo tablero
    tablero.resize(x);
    for (int i = 0; i < x; ++i) {
        tablero[i].resize(y);
        fill(tablero[i].begin(), tablero[i].end(), VACIO);
    }


    for (auto &coord: config.pos_rojo) {
        assert(es_color_libre(tablero[coord.first][coord.second])); //Compruebo que no haya otro jugador en esa posicion
        tablero[coord.first][coord.second] = ROJO; // guardo la posicion
    }

    for (auto &coord: config.pos_azul) {
        assert(es_color_libre(tablero[coord.first][coord.second]));
        tablero[coord.first][coord.second] = AZUL;
    }

    tablero[config.bandera_roja.first][config.bandera_roja.second] = BANDERA_ROJA;
    tablero[config.bandera_azul.first][config.bandera_azul.second] = BANDERA_AZUL;
    this->turno = ROJO;

    cout << "SE HA INICIALIZADO GAMEMASTER CON EXITO" << endl;
    // Insertar código que crea necesario de inicialización
    assert( this_game_strat == SECUENCIAL || this_game_strat == USTEDES || this_game_strat == RR || this_game_strat == SHORTEST);
    strat = this_game_strat;
}

void gameMaster::mover_jugador_tablero(coordenadas pos_anterior, coordenadas pos_nueva, color colorEquipo) {
    assert(es_color_libre(tablero[pos_nueva.first][pos_nueva.second]));
    tablero[pos_anterior.first][pos_anterior.second] = VACIO;
    tablero[pos_nueva.first][pos_nueva.second] = colorEquipo;
}


int gameMaster::mover_jugador(direccion dir, int nro_jugador) {
    //moviendo_jugador.lock();
    //cout<< "Soy el jugador " << nro_jugador << " del equipo " << turno << " y me acabo de mover "<< endl;
    //moviendo_jugador.unlock();
    return 0;
    /*coordenadas actual_pos;
    if (turno == AZUL) {
        actual_pos = pos_jugadores_azules[nro_jugador];
    } else {
        actual_pos = pos_jugadores_rojos[nro_jugador];
    }
    coordenadas apuntada_pos = proxima_posicion(actual_pos, dir);
    // Chequear que la movida sea valida
    assert(es_posicion_valida(apuntada_pos));
    // Que no se puedan mover 2 jugadores a la vez
    moviendo_jugador.lock();
    // Inicio Critica
    color apuntado_color = en_posicion(apuntada_pos);
    if (es_color_libre(apuntado_color)) {
        mover_jugador_tablero(actual_pos, apuntada_pos, turno);
    } else if (((apuntado_color == BANDERA_AZUL) && (turno == ROJO)) ||
               ((apuntado_color == BANDERA_ROJA) && (turno == AZUL))) {
        mover_jugador_tablero(actual_pos, apuntada_pos, turno);
        // setear la variable ganador
        ganador = turno;
        nro_ronda = 0;
    } else {
        // Devolver -1 simboliza que el jugador quizo moverse a un lugar y estaba ocupado
        return -1;
    }
    // Fin Critica
    moviendo_jugador.unlock();
    // Devolver acorde a la descripción
    return nro_ronda;*/
}


void gameMaster::termino_ronda(color equipo) {
    // FIXME: Hacer chequeo de que es el color correcto que está llamando
    // FIXME: Hacer chequeo que hayan terminado todos los jugadores del equipo o su quantum (via mover_jugador)
    // Hacer esto luego de checkear lo de arriba
    terminando_ronda.lock();
    if(equipo == turno){
        nro_ronda++;
        turno = (turno == ROJO) ? AZUL : ROJO;
        if(turno == ROJO){

            for (int i = 0; i < jugadores_por_equipos; ++i) {
                sem_post(&turno_rojo);
            }
        }
        else{
            for (int i = 0; i < jugadores_por_equipos; ++i) {
                sem_post(&turno_azul);
            }
        }
    }
    terminando_ronda.unlock();
}

bool gameMaster::termino_juego() {
    return ganador != INDEFINIDO;
}

coordenadas gameMaster::proxima_posicion(coordenadas anterior, direccion movimiento) {
    // Calcula la proxima posición a moverse (es una copia)
    switch (movimiento) {
        case (ARRIBA):
            anterior.second--;
            break;

        case (ABAJO):
            anterior.second++;
            break;

        case (IZQUIERDA):
            anterior.first--;
            break;

        case (DERECHA):
            anterior.first++;
            break;
    }
    return anterior; // está haciendo una copia por constructor
}

void gameMaster::comenzar_partida(){
    sem_init(&turno_rojo,0,0);
    sem_init(&turno_azul,0,0);

    //Empiezan los rojos
    for (int i = 0; i < jugadores_por_equipos; ++i) {
        sem_post(&turno_rojo);
    }
}


