#include <iostream>
#include <vector>
#include "gameMaster.h"
#include "equipo.h"
#include "definiciones.h"
#include "config.h"

using namespace std;

const estrategia strat = SECUENCIAL;
const int quantum = 10;
const int busqueda_distribuida = true;

int main() {
    Config config = *(new Config());
    gameMaster belcebu = gameMaster(config, strat);

    // Creo equipos (lanza procesos)
    Equipo rojo(&belcebu, ROJO, strat, config.cantidad_jugadores, quantum, config.pos_rojo, busqueda_distribuida);
    Equipo azul(&belcebu, AZUL, strat, config.cantidad_jugadores, quantum, config.pos_azul, busqueda_distribuida);

    rojo.comenzar();
    azul.comenzar();
    belcebu.comenzar_partida();
    rojo.terminar();
    azul.terminar();

    cout << "Bandera capturada por el equipo " << color(belcebu.ganador) << ". Felicidades!" << endl;
    belcebu.finalizar_partida();
    return 0;
}

