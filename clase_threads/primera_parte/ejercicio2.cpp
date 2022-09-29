#include <mutex>       // std::mutex
#include <thread>      // std::thread
#include <semaphore.h> // POSIX semaphores
#include <atomic>      // Atomic elements (std C++11)
#include <iostream>
#include <vector>
#include <functional>
#include <unistd.h>

using namespace std;

void saludar() {
    this_thread::sleep_for(500ms);
    //cout << "Hola soy un thread" << endl;
    write(1, "hola\n",5);
}

void crear_asignar_threads(vector<thread> &threads){
    //Creo los threads
    for (auto &t : threads) { 
        //Recordar que ref se utiliza para mantener una referencia de la variable pasada
        t = thread(saludar);
    }
}

// Hago join de los threads, de esta forma espero que terminen
void join_threads(vector<thread> &threads){
    for (auto &t : threads) { 
        t.join();
    }
}

// Hago detach de los threads
void join_detach(vector<thread> &threads){
    for (auto &t : threads) { 
        t.detach();
    }
}

int main() {
    vector<thread> threads(10);
    crear_asignar_threads(threads); 
    sleep(1);    
    //join_threads(threads);
    join_detach(threads);

    return 0;
}

/*

OPCION_1
terminate called without an active exception
Abortado (`core' generado)
    ENTONCES:
        Status 134. Significa "tuve que matar a los thread"

OPCION_2
Hola soy un threadHola soy un thread
Hola soy un threadHola soy un thread

Hola soy un thread
Hola soy un thread
Hola soy un thread
Hola soy un thread

Hola soy un thread
Hola soy un thread

y varia en cada intento

OPCION_3
No imprime nada, parece que no llega a ejecutar.

    Se queda corriendo, pero si el thread muere, ellos mueren tambien.
    Basicamente no marca como un error si el main termina. 
    Algo asi como "no me importa si se mueren"

*/

