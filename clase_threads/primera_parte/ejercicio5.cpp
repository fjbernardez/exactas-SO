#include <mutex>       // std::mutex
#include <thread>      // std::thread
#include <semaphore.h> // POSIX semaphores
#include <atomic>      // Atomic elements (std C++11)
#include <iostream>
#include <vector>
#include <functional>
#include <unistd.h>

using namespace std;

mutex mtx; 

void saludar(int contador) {
    
    for (size_t i = 0; i < 5; i++) {
        mtx.lock();
        cout << "Hola soy un thread: " << contador << endl;
        this_thread::sleep_for(100ms);
        cout << "Chau soy un thread: " << contador << endl;
        mtx.unlock();
        this_thread::sleep_for(200ms);

    }
}

void crear_asignar_threads(vector<thread> &threads, int contador){
    //Creo los threads
    for (auto &t : threads) { 
        //Recordar que ref se utiliza para mantener una referencia de la variable pasada
        t = thread(saludar, contador);
        contador++;
    }
}

// Hago join de los threads, de esta forma espero que terminen
void join_threads(vector<thread> &threads){
    for (auto &t : threads) { 
        t.join();
    }
}

int main() {
    vector<thread> threads(10);
    crear_asignar_threads(threads, 0); 
    //sleep(1);    
    join_threads(threads);

    return 0;
}


