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
    cout << "Hola soy un thread" << endl;
}

void crear_asignar_threads(vector<thread> &threads){
    //Creo los threads
    for (auto &t : threads) { 
        //Recordar que ref se utiliza para mantener una referencia de la variable pasada
        t = thread(saludar);
    }
}

int main() {
    vector<thread> threads(10);
    crear_asignar_threads(threads); 
    sleep(1);
    return 0;
}


