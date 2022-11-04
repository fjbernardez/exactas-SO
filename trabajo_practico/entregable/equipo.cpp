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
    //Hay barrera para asegurarse que todos los jugadores saben la posicion de antes de comenzar a moverse..
    //.. ya que algunos jugadores pueden terminar buscar_bandera_contraria sin saber donde esta
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


        //La idea es que todos los jugadores se quedan esperando a que belcebu les de..
        //.. permisos para jugar cuando sea su turno, el equipo rojo recibe al comienzo
        if (equipo == AZUL){
            sem_wait(&this->belcebu->turno_azul);
        }
        else{
            sem_wait(&this->belcebu->turno_rojo);
        }

        switch (this->strat) {
            //SECUENCIAL,RR,SHORTEST,USTEDES
            case (SECUENCIAL):
                //
                // ...
                //
                Equipo::jugar_turno_estrategia_secuencial(nro_jugador);

                break;

            case (RR):
                //
                // ...
                //
                Equipo::jugar_turno_estrategia_rr(nro_jugador);
                break;

            case (SHORTEST):
                //
                // ...
                //
                Equipo::jugar_turno_estrategia_shortest(nro_jugador);
                break;

            case (USTEDES):
                //
                // ...
                //
                Equipo::jugar_turno_estrategia_ustedes(nro_jugador);
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
    sem_destroy(&equipo_coordinacion_sem_entrada);
    sem_destroy(&equipo_coordinacion_sem_salida);
    sem_destroy(&equipo_coordinacion_sem_bandera);

    for (int i = 0; i < cant_jugadores; ++i) {
        sem_destroy(&rr_coordinacion_sem[i]);
    }
}

coordenadas Equipo::buscar_bandera_contraria(int nro_jugador) {
    //
    // ...
    //

    //Version distribuida equitativamente
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

void Equipo::jugar_turno_estrategia_secuencial(int nro_jugador){
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

    //Nos aseguramos que todos los jugadores esten listos para jugar este turno
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

    int nro_ronda = this->belcebu->mover_jugador(DERECHA, nro_jugador);

    //Si todos movieron, terminamos turno
    equipo_coordinacion_mutex.lock();
    cant_jugadores_que_ya_jugaron++;
    if (cant_jugadores_que_ya_jugaron == cant_jugadores){
        //belcebu le va a dar permisos al proximo equipo;
        cant_jugadores_que_ya_jugaron = 0;
        for (int i = 0; i < cant_jugadores; ++i) {
            sem_post(&equipo_coordinacion_sem_salida);
        }
        this->belcebu->termino_ronda(equipo);
    }
    equipo_coordinacion_mutex.unlock();
    sem_wait(&equipo_coordinacion_sem_salida);

}
void Equipo::jugar_turno_estrategia_rr(int nro_jugador) {

    //Nos aseguramos que todos los jugadores esten listos para jugar este turno
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
    int quantum_que_habra_cuando_me_toque = quantum - nro_jugador;
    int nro_ronda;

    while(quantum_que_habra_cuando_me_toque > 0) {
        //El array_de_sem se inicializa en equipo.comienza
        sem_wait(&rr_coordinacion_sem[nro_jugador]);

        //Aca se esperaría que haya solo un jugador por vez..
        // .. no debería ser necesario el mutex

        equipo_coordinacion_mutex.lock();
        quantum_que_habra_cuando_me_toque = quantum_restante - cant_jugadores;
        nro_ronda = belcebu->mover_jugador(DERECHA, nro_jugador);

        quantum_restante--;

        //Si queda quantum, hay un proximo jugador esperando a que lo despierte
        if(quantum_restante > 0){
            sem_post(&rr_coordinacion_sem[(nro_jugador + 1) % cant_jugadores]);
        }
        equipo_coordinacion_mutex.unlock();
    }

    //barrera para ver que todos terminaron de moverse
    equipo_coordinacion_mutex.lock();
    cant_jugadores_que_ya_jugaron++;
    if (cant_jugadores_que_ya_jugaron == cant_jugadores){
        //belcebu le va a dar permisos al proximo equipo;
        belcebu->termino_ronda(equipo);
        for (int i = 0; i < cant_jugadores; ++i) {
            sem_post(&equipo_coordinacion_sem_salida);
        }
        cant_jugadores_que_ya_jugaron = 0;
        quantum_restante = quantum;
    }
    equipo_coordinacion_mutex.unlock();
    sem_wait(&equipo_coordinacion_sem_salida);

}

void Equipo::jugar_turno_estrategia_shortest(int nro_jugador) {

    //todos los jugadores buscan si son el que esta más cerca de la bandera
    //Solo si son el más cercano, se mueven y llaman a terminar turno

    //nro_jugador_cercano se debe hacer funcion que compare con donde esta la bandera
    int nro_jugador_cercano = 0;
    int nro_ronda;
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
}
void Equipo::jugar_turno_estrategia_ustedes(int nro_jugador) {
    //Dejo una version con estrategia: Se mueve Quantum veces el jugador más cercano al principio el turno

    int nro_jugador_cercano = 0;
    int nro_ronda;
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
        while (quantum_restante > 0){
            nro_ronda = belcebu->mover_jugador(DERECHA, nro_jugador);
            quantum_restante--;
        }

    }

    equipo_coordinacion_mutex.lock();
    cant_jugadores_que_ya_jugaron++;
    if (cant_jugadores_que_ya_jugaron == cant_jugadores){
        for (int i = 0; i < cant_jugadores; ++i) {
            sem_post(&equipo_coordinacion_sem_salida);
        }
        belcebu->termino_ronda(equipo);
        cant_jugadores_que_ya_jugaron = 0;
        quantum_restante = quantum;
    }
    equipo_coordinacion_mutex.unlock();
    sem_wait(&equipo_coordinacion_sem_salida);

}