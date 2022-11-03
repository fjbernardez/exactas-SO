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


    //Aca deberían hacer lo de buscar bandera
    coordenadas pos_bandera_encontrada = buscar_bandera_contraria(nro_jugador);
    equipo_coordinacion_mutex.lock();
    cant_jugadores_ya_saben_bandera++;
    if (cant_jugadores_ya_saben_bandera == cant_jugadores){
        for (int i = 0; i < cant_jugadores; ++i) {
            sem_post(&equipo_coordinacion_sem_bandera);
        }
    }
    equipo_coordinacion_mutex.unlock();
    sem_wait(&equipo_coordinacion_sem_bandera);

    while (!this->belcebu->termino_juego()) { // Chequear que no haya una race condition en gameMaster

        // Numero de intento puede ser algo para que el jugador decida donde moverse si..
        //.. fue bloqueado la primera vez
        int nro_intento = -1;
        int nro_ronda = -1;
        int quantum_que_habra_cuando_me_toque;
        int nro_jugador_cercano;
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

                }*/

                //Nos aseguramos que todos los jugadores del turno actual esten para comenzar a moverse
                equipo_coordinacion_mutex.lock();
                cant_jugadores_listos_para_jugar++;
                if (cant_jugadores_listos_para_jugar == cant_jugadores){
                    cant_jugadores_listos_para_jugar = 0;
                    for (int i = 0; i < cant_jugadores; ++i) {
                        sem_post(&equipo_coordinacion_sem_entrada);
                    }
                }
                equipo_coordinacion_mutex.unlock();
                sem_wait(&equipo_coordinacion_sem_entrada);

                nro_ronda = belcebu->mover_jugador(DERECHA, nro_jugador);

                //Si todos movieron, terminamos turno
                equipo_coordinacion_mutex.lock();
                cant_jugadores_que_ya_jugaron++;
                if (cant_jugadores_que_ya_jugaron == cant_jugadores){
                    //belcebu le va a dar permisos al proximo equipo;
                    cant_jugadores_que_ya_jugaron = 0;
                    for (int i = 0; i < cant_jugadores; ++i) {
                        sem_post(&equipo_coordinacion_sem_salida);
                    }
                    belcebu->termino_ronda(equipo);
                }
                equipo_coordinacion_mutex.unlock();
                sem_wait(&equipo_coordinacion_sem_salida);

                break;

            case (RR):
                //
                // ...
                //

                //Nos aseguramos que todos los jugadores esten listos para jugar
                equipo_coordinacion_mutex.lock();
                cant_jugadores_listos_para_jugar++;
                if (cant_jugadores_listos_para_jugar == cant_jugadores){

                    for (int i = 0; i < cant_jugadores; ++i) {
                        sem_post(&equipo_coordinacion_sem_entrada);
                    }
                    //Le habilitamos el semaforo al primer jugador de la ronda
                    sem_post(&rr_coordinacion_sem[0]);
                    cant_jugadores_listos_para_jugar = 0;
                }
                equipo_coordinacion_mutex.unlock();
                sem_wait(&equipo_coordinacion_sem_entrada);

                //Si cuando le toque de nuevo al jugador hay quantum positivo, va a moverse, sino no
                quantum_que_habra_cuando_me_toque = quantum - nro_jugador;

                while(quantum_que_habra_cuando_me_toque > 0) {
                    //El array_de_sem se inicializa en equipo.comienza
                    sem_wait(&rr_coordinacion_sem[nro_jugador]);

                    //Aca se esperaría que haya solo un jugador por vez..
                    // .. no debería ser necesario el mutex

                    equipo_coordinacion_mutex.lock();
                    quantum_que_habra_cuando_me_toque = quantum_restante - cant_jugadores;
                    nro_ronda = belcebu->mover_jugador(DERECHA, nro_jugador);

                    quantum_restante--;

                    //Si queda quantum, hay un proximo jugadores esperando a que lo despierte
                    if(quantum_restante > 0){

                        sem_post(&rr_coordinacion_sem[(nro_jugador + 1) % cant_jugadores]);
                    }
                    equipo_coordinacion_mutex.unlock();

                }

                equipo_coordinacion_mutex.lock();
                cant_jugadores_que_ya_jugaron++;
                if (cant_jugadores_que_ya_jugaron == cant_jugadores){
                    //belcebu le va a dar permisos al proximo equipo;
                    quantum_restante = quantum;
                    belcebu->termino_ronda(equipo);
                    for (int i = 0; i < cant_jugadores; ++i) {
                        sem_post(&equipo_coordinacion_sem_salida);
                    }
                    cant_jugadores_que_ya_jugaron = 0;
                }
                equipo_coordinacion_mutex.unlock();
                sem_wait(&equipo_coordinacion_sem_salida);
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

                //nro_jugador_cercano se debe hacer funcion que compare con donde esta la bandera
                nro_jugador_cercano = 0;
                equipo_coordinacion_mutex.lock();
                cant_jugadores_listos_para_jugar++;
                if (cant_jugadores_listos_para_jugar == cant_jugadores){
                    for (int i = 0; i < cant_jugadores; ++i) {
                        sem_post(&equipo_coordinacion_sem_entrada);
                    }
                    cant_jugadores_listos_para_jugar = 0;
                }
                equipo_coordinacion_mutex.unlock();
                sem_wait(&equipo_coordinacion_sem_entrada);


                if(nro_jugador == nro_jugador_cercano){
                    nro_ronda = belcebu->mover_jugador(DERECHA, nro_jugador);
                }

                equipo_coordinacion_mutex.lock();
                cant_jugadores_que_ya_jugaron++;
                if (cant_jugadores_que_ya_jugaron == cant_jugadores){
                    for (int i = 0; i < cant_jugadores; ++i) {
                        sem_post(&equipo_coordinacion_sem_salida);
                    }
                    belcebu->termino_ronda(equipo);
                    cant_jugadores_que_ya_jugaron = 0;
                }
                equipo_coordinacion_mutex.unlock();
                sem_wait(&equipo_coordinacion_sem_salida);


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
    tam_X = belcebu->getTamx();
    tam_Y = belcebu->getTamy();
    pos_bandera_contraria = make_pair(-1,-1);
    //Se puede separar las inicializacion de sem_t con un switch case por strategia, pero no es 100% necesario
    sem_init(&equipo_coordinacion_sem_entrada, 0, 0);
    sem_init(&equipo_coordinacion_sem_salida, 0, 0);
    sem_init(&equipo_coordinacion_sem_bandera, 0, 0);
    //Se usan en RR
    for (int i = 0; i < cant_jugadores; ++i) {
        sem_init(&rr_coordinacion_sem[i], 0, 0);
    }


    // Creamos los jugadores
    for (int i = 0; i < cant_jugadores; i++) {
        jugadores.emplace_back(thread(&Equipo::jugador, this, i));
    }
}

void Equipo::terminar() {
    for (auto &t: jugadores) {
        t.join();
    }
    sem_close(&equipo_coordinacion_sem_entrada);
    sem_close(&equipo_coordinacion_sem_salida);
    sem_close(&equipo_coordinacion_sem_bandera);

    for (int i = 0; i < cant_jugadores; ++i) {
        sem_close(&rr_coordinacion_sem[i]);
    }
}

coordenadas Equipo::buscar_bandera_contraria(int nro_jugador) {
    //
    // ...
    //

    //Version distribuida
    int i = nro_jugador+1;
    bool no_se_encontro = true;
    while(no_se_encontro && i <tam_Y){
        coordenadas pos_mirando;
        if(equipo==ROJO){
            pos_mirando =make_pair(99,i);
        }
        else{
            pos_mirando =make_pair(1,i);
        }
        color color_mirando = belcebu->en_posicion(pos_mirando);
        if(color_mirando == bandera_contraria){
            //Creo que aca frenaria el clock time
            pos_bandera_contraria = pos_mirando;
        }
        equipo_coordinacion_mutex.lock();
        no_se_encontro = (pos_bandera_contraria == make_pair(-1,-1));
        equipo_coordinacion_mutex.unlock();
        i+= cant_jugadores;
    }
    return pos_bandera_contraria;

    //Version no distribuida
    /*
     *
    buscar_bandera_i; TIENE QUE SER VARIABLE COMPARTIDA POR EL EQUIPO INICIALIZADO EN 0
    bool no_se_encontro = true;
    equipo_coordinacion_mutex.lock();
    buscar_bandera_i ++;
    int i = buscar_bandera;
    equipo_coordinacion_mutex.unlock();

    while(no_se_encontro && i <tam_Y){
        coordenadas pos_mirando;
        if(equipo==ROJO){
            pos_mirando =make_pair(99,i);
        }
        else{
            pos_mirando =make_pair(1,i);
        }
        color color_mirando = belcebu->en_posicion(pos_mirando);
        if(color_mirando == bandera_contraria){
            equipo_coordinacion_mutex.lock();
            pos_bandera_contraria = pos_mirando;
            equipo_coordinacion_mutex.unlock();
        }
        equipo_coordinacion_mutex.lock();
        no_se_encontro = (pos_bandera_contraria == make_pair(-1,-1));
        buscar_bandera_i ++;
        i = buscar_bandera;
        equipo_coordinacion_mutex.unlock();
    }
    return pos_bandera_contraria;
     */
}
