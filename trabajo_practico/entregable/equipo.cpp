#include "equipo.h"


direccion Equipo::apuntar_a(coordenadas pos1, coordenadas pos2) {
    if (pos2.second > pos1.second) return ABAJO;
    if (pos2.second < pos1.second) return ARRIBA;
    if (pos2.first > pos1.first) return DERECHA;
    if (pos2.first < pos1.first) return IZQUIERDA;
}


void Equipo::jugador(int nro_jugador) {
    //
    // ...
    //

    while (!this->belcebu->termino_juego()) { // Chequear que no haya una race condition en gameMaster
        // Numero de intento puede ser algo para que el jugador decida donde moverse si..
        //.. fue bloqueado la primera vez
        int nro_intento = -1;
        int nro_ronda = -1;
        switch (this->strat) {
            //SECUENCIAL,RR,SHORTEST,USTEDES
            case (SECUENCIAL):
                //
                // ...
                //

                //La idea es que todos los jugadores se quedan esperando a que belcebu les de..
                //.. permisos para pasar, el equipo rojo ya comienza con permisos para sus jugadores
                if (equipo == AZUL){
                    sem_wait(&belcebu->turno_azul);
                }
                else{
                    sem_wait(&belcebu->turno_rojo);
                }
                nro_intento = -1;
                nro_ronda = -1;
                //Seccion donde solo entran los jugadores si es el turno de su equipo
                while(nro_ronda == -1){
                    nro_intento ++;
                    //TODO: una funcion más inteligente para decidir dir_a_mover
                    direccion dir_a_mover;
                    if(equipo == ROJO){
                        dir_a_mover = (nro_intento == 0) ? DERECHA : ARRIBA;
                    }
                    else{
                        dir_a_mover = (nro_intento == 0) ? ARRIBA : IZQUIERDA;
                    }

                    nro_ronda = belcebu->mover_jugador(dir_a_mover, nro_jugador);
                }

                secuencial_mutex.lock();


                cant_jugadores_que_ya_jugaron++;
                if (cant_jugadores_que_ya_jugaron == cant_jugadores){
                    //belcebu le va a dar permisos al proximo equipo;

                    belcebu->termino_ronda(equipo);
                    cant_jugadores_que_ya_jugaron = 0;
                }
                secuencial_mutex.unlock();


                break;

            case (RR):
                //
                // ...
                //
                break;

            case (SHORTEST):
                //
                // ...
                //
                break;

            case (USTEDES):
                //
                // ...
                //
                break;
            default:
                break;
        }
        // Termino ronda ? Recordar llamar a belcebu...
        // OJO. Esto lo termina un jugador...
        //
        // ...
        //
    }

}

Equipo::Equipo(gameMaster *belcebu, color equipo,
               estrategia strat, int cant_jugadores, int quantum, vector<coordenadas> posiciones) {
    this->belcebu = belcebu;
    this->equipo = equipo;
    this->contrario = (equipo == ROJO) ? AZUL : ROJO;
    this->bandera_contraria = (equipo == ROJO) ? BANDERA_AZUL : BANDERA_ROJA;
    this->strat = strat;
    this->quantum = quantum;
    this->quantum_restante = quantum;
    this->cant_jugadores = cant_jugadores;
    this->posiciones = posiciones;
    //
    // ...
    //
}

void Equipo::comenzar() {
    // Arranco cuando me toque el turno
    // TODO: Quien empieza ?
    //
    // ...
    //

    // Creamos los jugadores
    for (int i = 0; i < cant_jugadores; i++) {
        jugadores.emplace_back(thread(&Equipo::jugador, this, i));
    }
}

void Equipo::terminar() {
    for (auto &t: jugadores) {
        t.join();
    }
}

coordenadas Equipo::buscar_bandera_contraria() {
    //
    // ...
    //
}
