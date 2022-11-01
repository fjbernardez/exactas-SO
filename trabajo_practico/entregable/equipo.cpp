#include "equipo.h"
#include <assert.h>

direccion Equipo::apuntar_a(coordenadas pos1, coordenadas pos2) {
    assert(pos1 != pos2);
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
        int xd = 0;

        //La idea es que todos los jugadores se quedan esperando a que belcebu les de..
        //.. permisos para jugar cuando sea su turno, el equipo rojo ya comienza con permisos para sus jugadores
        if (equipo == AZUL){
            sem_wait(&belcebu->turno_azul);
        }
        else{
            sem_wait(&belcebu->turno_rojo);
        }

        switch (this->strat) {
            //SECUENCIAL,RR,SHORTEST,USTEDES
            case (SECUENCIAL):
                //
                // ...
                //



                //Moverse
                /*nro_intento = -1;
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
                        dir_a_mover = (nro_intento == 0) ? IZQUIERDA : ABAJO;
                    }
                    //nro_ronda devuelve -1 si el lugar al que se quizo mover estaba bloqueado
                    nro_ronda = belcebu->mover_jugador(dir_a_mover, nro_jugador);
                }*/


                equipo_coordinacion_mutex.lock();
                cant_jugadores_que_ya_jugaron++;
                if (cant_jugadores_que_ya_jugaron == cant_jugadores){
                    //belcebu le va a dar permisos al proximo equipo;
                    cant_jugadores_que_ya_jugaron = 0;
                    belcebu->termino_ronda(equipo);
                    for (int i = 0; i < cant_jugadores; ++i) {
                        sem_post(&equipo_coordinacion_sem);
                    }
                }
                equipo_coordinacion_mutex.unlock();

                sem_wait(&equipo_coordinacion_sem);

                break;

            case (RR):
                //
                // ...
                //

                //El array_de_mutex se inicializa en equipo.comienza, y le da al primer jugador permiso
                sem_wait(&rr_coordinacion_sem[nro_jugador]);

                //Se mueve o intenta mover adonde pueda

                //Si el Quantum_restante es cero, llama a terminar turno y le de permiso al PRIMER jugador, preparando para la proxima ronda..
                //.. resetea quatum restante a quantum
                quantum_restante--;
                if(quantum_restante == 0){
                    quantum_restante = quantum;
                    belcebu->termino_ronda(equipo);
                    sem_post(&rr_coordinacion_sem[0]);
                }
                else{
                    //Si el Quantum_restante es mayor que cero, le da permiso al SIGUIENTE jugador
                    sem_post(&rr_coordinacion_sem[(nro_jugador+1)%cant_jugadores]);
                }
                break;

            case (SHORTEST):
                //
                // ...
                //

                //todos los jugadores buscan si son el que esta más cerca de la bandera
                //Solo si son el más cercano, se mueven y llaman a terminar turno


                //El equipo_mas_cercano_sem semaforo es opcional? Solo es necesario por el caso justo donde hay empatados..
                //.. al principio del turno, va primero el thread que est más cerca por alguna razon se mueve alejandose de la bandera..
                //.. luego viene el que estaba empayado ahora es el más cercano y se mueve tambien, hay dos movimientos en un turno

                //nro_jugador_cercano = jugador_mas_cercano();
                equipo_coordinacion_mutex.lock();
                cant_jugadores_que_ya_saben_mas_cercano++;
                if (cant_jugadores_que_ya_saben_mas_cercano == cant_jugadores){
                    //belcebu le va a dar permisos al proximo equipo;
                    cant_jugadores_que_ya_saben_mas_cercano = 0;
                    for (int i = 0; i < cant_jugadores; ++i) {
                        sem_post(&equipo_mas_cercano_sem);
                    }
                }
                equipo_coordinacion_mutex.unlock();
                sem_wait(&equipo_mas_cercano_sem);

                //if(nro_jugador == nro_jugador_cercano()){
                    // Moverse
                    // belcebu->termino_ronda(equipo);
                //}

                equipo_coordinacion_mutex.lock();
                cant_jugadores_que_ya_jugaron++;
                if (cant_jugadores_que_ya_jugaron == cant_jugadores){

                    cant_jugadores_que_ya_jugaron = 0;
                    for (int i = 0; i < cant_jugadores; ++i) {
                        sem_post(&equipo_coordinacion_sem);
                    }
                }
                equipo_coordinacion_mutex.unlock();
                sem_wait(&equipo_coordinacion_sem);


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

    this->rr_coordinacion_sem = vector<sem_t> (cant_jugadores);
}

void Equipo::comenzar() {
    //...

    //Se puede separar las inicializacion de sem_t con un switch case por strategia, pero no es 100% necesario

    //Se usa en Secuencial y Shortest
    sem_init(&equipo_coordinacion_sem, 0, 0);
    //Se usa en Shortest
    sem_init(&equipo_mas_cercano_sem, 0, 0);
    //Se usan en RR
    for (int i = 0; i < cant_jugadores; ++i) {
        sem_init(&rr_coordinacion_sem[i], 0, 0);
    }
    sem_post(&rr_coordinacion_sem[0]);

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
