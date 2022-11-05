#include "equipo.h"
#include <assert.h>
#include <time.h>

direccion Equipo::apuntar_a(coordenadas pos1, coordenadas pos2) {
    assert(pos1 != pos2);
    if (pos2.second > pos1.second) return ABAJO;
    if (pos2.second < pos1.second) return ARRIBA;
    if (pos2.first > pos1.first) return DERECHA;
    if (pos2.first < pos1.first) return IZQUIERDA;
}


void Equipo::jugador(int nro_jugador) {
    //Hay barrera para asegurarse que todos los jugadores saben donde esta la bandera de antes de comenzar a moverse..
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
        if (equipo == AZUL) {
            sem_wait(&this->belcebu->turno_azul);
        } else {
            sem_wait(&this->belcebu->turno_rojo);
        }
        if (!this->belcebu->termino_juego()) {
        switch (this->strat) {
            //SECUENCIAL,RR,SHORTEST,USTEDES
            case (SECUENCIAL):
                Equipo::jugar_turno_estrategia_secuencial(nro_jugador);
                break;
            case (RR):
                Equipo::jugar_turno_estrategia_rr(nro_jugador);
                break;
            case (SHORTEST):
                Equipo::jugar_turno_estrategia_shortest(nro_jugador);
                break;
            case (USTEDES):
                Equipo::jugar_turno_estrategia_ustedes(nro_jugador);
                break;
            default:
                break;
            }
        }
        // Termino ronda ? Recordar llamar a belcebu...
        // OJO. Esto lo termina un jugador...
    }

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

Equipo::Equipo(gameMaster *belcebu, color equipo,
               estrategia strat, int cant_jugadores, int quantum, vector<coordenadas> posiciones,bool busqueda_distribuida) {
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
    this->busqueda_distribuida = busqueda_distribuida;
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
    start.tv_nsec = 0;
    start.tv_sec = 0;
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
    clock_coordinacion_mutex.lock();
    if(start.tv_sec == 0 && start.tv_nsec == 0){
        clock_gettime(CLOCK_REALTIME,&start);
    }
    clock_coordinacion_mutex.unlock();
    //Version distribuida equitativamente, cada jugador revisa una cantidad determinada de casillas
    if(busqueda_distribuida){
        int i = nro_jugador+1;
        bool no_se_encontro = true;
        coordenadas pos_mirando;
        color color_mirando;
        while(i < tam_Y){

            if(equipo==ROJO){
                pos_mirando =make_pair(this->tam_X - 1,i);
            }
            else{
                pos_mirando =make_pair(1,i);
            }
            color_mirando = belcebu->en_posicion(pos_mirando);
            if(color_mirando == bandera_contraria){
                clock_gettime(CLOCK_REALTIME,&finish);
                string col = this->equipo == 0 ? "AZUL" : "ROJO";
                string print = "El equipo " + col +
                        " encontro la bandera en tiempo " + to_string(finish.tv_sec - start.tv_sec) + "sec " +
                        to_string(finish.tv_nsec - start.tv_nsec) + "nsec en la posicion " + to_string(pos_mirando.first) + " " + to_string(pos_mirando.second);
                cout << print << endl;
                pos_bandera_contraria = pos_mirando;
            }
            i+= cant_jugadores;
        }
    }
    //Version no distribuida, van a ir revisando casillas los jugadores que el scheduler decida
    else{
        bool no_se_encontro = true;
        equipo_coordinacion_mutex.lock();
        buscar_bandera_i ++;
        int i = buscar_bandera_i;
        equipo_coordinacion_mutex.unlock();
        coordenadas pos_mirando;
        color color_mirando;
        while(i <tam_Y){
            if(equipo==ROJO){
                pos_mirando =make_pair(99,i);
            }
            else{
                pos_mirando =make_pair(1,i);
            }
            color_mirando = belcebu->en_posicion(pos_mirando);
            if(color_mirando == bandera_contraria){
                clock_gettime(CLOCK_REALTIME,&finish);
                string col = this->equipo == 0 ? "AZUL" : "ROJO";
                string print = "El equipo " + col +
                               " encontro la bandera en tiempo " + to_string(finish.tv_sec - start.tv_sec) + "sec " +
                               to_string(finish.tv_nsec - start.tv_nsec) + "nsec en la posicion " + to_string(pos_mirando.first) + " " + to_string(pos_mirando.second);
                cout << print << endl;
                clock_gettime(CLOCK_REALTIME,&finish);
                pos_bandera_contraria = pos_mirando;
            }
            equipo_coordinacion_mutex.lock();
            buscar_bandera_i ++;
            i = buscar_bandera_i;
            equipo_coordinacion_mutex.unlock();
        }
    }

    return pos_bandera_contraria;
}

void Equipo::jugar_turno_estrategia_secuencial(int nro_jugador){

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

    //El jugador intentara moverse, si no tiene lugares disponibles terminara su turno
    int nro_ronda = jugador_moverse(nro_jugador);

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

        quantum_que_habra_cuando_me_toque = quantum_restante - cant_jugadores;
        nro_ronda = jugador_moverse(nro_jugador);

        quantum_restante--;

        //Si queda quantum, hay un proximo jugador esperando a que lo despierte
        if(quantum_restante > 0){
            sem_post(&rr_coordinacion_sem[(nro_jugador + 1) % cant_jugadores]);
        }
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
        nro_ronda = jugador_moverse(nro_jugador);
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
            nro_ronda = jugador_moverse(nro_jugador);
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

//Con esta funcion devolvemos una lista de direcciones, ordenadas por optimalidad de llevarnos de pos1 a pos2
vector<direccion> Equipo::mejores_posiciones(coordenadas pos1, coordenadas pos2){
    vector<direccion> vertical;
    vector<direccion> horizontal;
    int horDist = pos1.first - pos2.first;
    int verDist = pos1.second - pos2.second;
    if(horDist > 0){
        horizontal = {IZQUIERDA, DERECHA};
    } else{
        horizontal = {DERECHA, IZQUIERDA};
    }
    if(verDist > 0){
        vertical = {ARRIBA, ABAJO};
    } else {
        vertical = {ABAJO, ARRIBA};
    }
    if(abs(verDist) > abs(horDist)){
        return {vertical[0], horizontal[0], vertical[1], horizontal[1]};
    } else {
        return {horizontal[0], vertical[0], horizontal[1], vertical[1]};
    }
}

//El jugador busca las posibles direcciones a las que se puede mover, e intenta moverse a ellas en orden optimo
//El jugador puede ir en direccion contraria a la optima si estan ocupadas las posiciones optimas
int Equipo::jugador_moverse(int nro_jugador){
    int nro_ronda = -1;
    coordenadas mi_posicion = posiciones[nro_jugador];
    vector<direccion> mis_direcciones_optimas = mejores_posiciones(mi_posicion,pos_bandera_contraria);
    int intento = 0;
    direccion probando_direccion;
    coordenadas probando_coordenada;
    while(nro_ronda == -1 and intento < 4){
        probando_direccion = mis_direcciones_optimas[intento];
        probando_coordenada = this->belcebu->proxima_posicion(mi_posicion,probando_direccion);
        if(this->belcebu->es_posicion_valida(probando_coordenada)){
            nro_ronda = this->belcebu->mover_jugador(mis_direcciones_optimas[intento],nro_jugador);
        }
        intento++;
    }
    equipo_coordinacion_mutex.lock();
    //Si dice ronda -1 es que no se pudo mover
    if(!this->belcebu->termino_juego() || probando_coordenada == this->pos_bandera_contraria) {
        string col = this->equipo == 0 ? "AZUL" : "ROJO";
        string print = "Soy el jugador " + to_string(nro_jugador) +
                " del equipo " + col + " y me acabo de mover en la ronda " + to_string(nro_ronda);
        cout << print << endl;
        string print2 = "Me movi a la posicion " + to_string(probando_coordenada.first) + " " + to_string(probando_coordenada.second);
        cout <<  print2 << endl;
    }
    equipo_coordinacion_mutex.unlock();
    if(nro_ronda >-1){
        //Se pudo mover el jugador, debe actualizar su posicion en el vector de posiciones del equipo
        posiciones[nro_jugador] = probando_coordenada;
    }
    return nro_ronda;
}