#include <iostream>
#include <vector>
#include "gameMaster.h"
#include "equipo.h"
#include "definiciones.h"
#include "config.h"
#include "string.h"

using namespace std;

estrategia strat = SECUENCIAL;
const char *config_filename = "./config/config_parameters.csv";
const int quantum = 10;
const int busqueda_distribuida = true;

inline void print_usage() {
	cout << "Uso: test_tp_sistemas [-config filename] [-strat secuencial|rr|shortest|custom]" << endl;
}

int main(int argc, char **argv) {
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-strat") == 0) {
			if (++i >= argc) {
				cerr << "ERROR: Falta estraregia luego de -strat" << endl;
				print_usage();
				return 1;
			}
			else if (strcmp(argv[i], "secuencial") == 0)
				strat = SECUENCIAL;
			else if (strcmp(argv[i], "rr") == 0)
				strat = RR;
			else if (strcmp(argv[i], "shortest") == 0)
				strat = SHORTEST;
			else if (strcmp(argv[i], "custom") == 0)
				strat = USTEDES;
			else {
				cerr << "ERROR: Estrategia \"" << argv[i] << "\" no reconocida." << endl;
				return 1;
			}
		}
		else if (strcmp(argv[i], "-config") == 0) {
			if (++i >= argc) {
				cerr << "ERROR: Falta nombre de archivo luego de -config" << endl;
				print_usage();
				return 1;
			}
			else
				config_filename = argv[i];
		}
		else {
			cerr << "ERROR: Argumento invalido: \"" << argv[i] << "\"" << endl;
			print_usage();
			return 1;
		}
	}
    Config config = *(new Config(config_filename));
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

