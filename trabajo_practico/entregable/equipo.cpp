#include "equipo.h"


direccion Equipo::apuntar_a(coordenadas pos1, coordenadas pos2) {
    if (pos2.second > pos1.second) return ABAJO;
    if (pos2.second < pos1.second) return ARRIBA;
    if (pos2.first > pos1.first) return DERECHA;
    if (pos2.first < pos1.first) return IZQUIERDA;
}

bool Equipo::puedeMover(coordenadas pos){
    bool res = this->belcebu->es_posicion_valida(pos);
    res = res && this->belcebu->en_posicion(pos) == VACIO;
    return res;
}

int Equipo::distancia(coordenadas pos1, coordenadas pos2){
    int res = abs(pos1.first - pos2.first) + abs(pos1.second - pos2.second);
    return res;
}

int Equipo::jugadorMasCercano(){
    int res = 0;
    int distanciaMasCercano = distancia(this->posiciones[0], this->pos_bandera_contraria);
    for(int i = 1; i < this->cant_jugadores; i++){
        int dist = distancia(this->posiciones[i], this->pos_bandera_contraria);
        if(dist < distanciaMasCercano){
            res = i;
            distanciaMasCercano = dist;
        }
    }
    return res;
}


void Equipo::jugador(int nro_jugador) {
    //
    // ...
    //

    while (!this->belcebu->termino_juego()) { // Chequear que no haya una race condition en gameMaster
        switch (this->strat) {
            //SECUENCIAL,RR,SHORTEST,USTEDES
            case (SECUENCIAL):
                //
                // ...
                //
                /*
<<<<<<< Updated upstream
=======*/
                // Mutex

                // Aseguramos una dirección para que se pueda mover
                direccion dir_a_mover = this->apuntar_a(this->posiciones[nro_jugador], this->pos_bandera_contraria);
                bool pudoMover = false;
                int i = 0;
                while(i < 4 && pudoMover){
                    // newPos seria la posicion a la que se moveria si hiciera el movimiento en dir_a_mover
                    dir_a_mover = direccion((dir_a_mover + i) % 4);
                    coordenadas newPos = make_pair(this->posiciones[i].first - dir_a_mover == ARRIBA + dir_a_mover == ABAJO,
                                                   this->posiciones[i].second - dir_a_mover == IZQUIERDA + dir_a_mover == DERECHA);
                    if(this->belcebu->es_posicion_valida(newPos)){
                        pudoMover = true;
                    }
                }

                if(i < 4){
                    direccion dir = direccion(dir_a_mover);
                    this->belcebu->mover_jugador(dir, nro_jugador);
                }

                // End of mutex


                //Moverse
                /*nro_intento = -1;
                nro_ronda = -1;
                //Seccion donde solo entran los jugadores si es el turno de su equipo
                while(nro_ronda == -1){
                    nro_intento ++;
                    //TODO: una funcion mas inteligente para decidir dir_a_mover
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

>>>>>>> Stashed changes
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
