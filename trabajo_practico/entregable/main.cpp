#include <iostream>
#include <thread>
#include <sys/unistd.h>
#include <vector>
#include <mutex>
#include "gameMaster.h"
#include "equipo.h"
#include "definiciones.h"
#include "config.h"

using namespace std;

const estrategia strat = RR;
const int quantum = 10;

int main() {
    Config config = *(new Config());


    gameMaster belcebu = gameMaster(config,strat);


    // Creo equipos (lanza procesos)
    Equipo rojo(&belcebu, ROJO, strat, config.cantidad_jugadores, quantum, config.pos_rojo);
    Equipo azul(&belcebu, AZUL, strat, config.cantidad_jugadores, quantum, config.pos_azul);

    rojo.comenzar();
    azul.comenzar();
    belcebu.comenzar_partida();
    rojo.terminar();
    azul.terminar();



    cout << "Bandera capturada por el equipo " << belcebu.ganador << ". Felicidades!" << endl;

}

