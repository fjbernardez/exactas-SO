#include <mutex>       // std::mutex
#include <thread>      // std::thread
#include <semaphore.h> // POSIX semaphores
#include <atomic>      // Atomic elements (std C++11)
#include <iostream>
#include <vector>
#include <functional>
#include <unistd.h>
#include <numeric>

#define MAX 100

using namespace std;

vector<int> v1(MAX);
vector<int> v2(MAX);
vector<int> v3(MAX);

//sem_t semA;
//sem_t semB;
vector<thread> threads(5);

void multiplicar(int inicio, int fin) {
    for (int i = inicio; i < fin; i++) {
        v3[i] = v1[i] * v2 [i];   
    }    
}

void crear_asignar_threads(){
    threads[0] = thread(multiplicar,  0,  20);
    threads[1] = thread(multiplicar, 20,  40);
    threads[2] = thread(multiplicar, 40,  60);
    threads[3] = thread(multiplicar, 60,  80);
    threads[4] = thread(multiplicar, 80, 100);
}

// Hago join de los threads, de esta forma es1
void join_threads(vector<thread> &threads){
    for (auto &t : threads) { 
        t.join();
    }
}
int main() {
    //sem_init(&semA, 0, 0);
    //sem_init(&semB, 0, 0);
    iota(v1.begin(), v1.end(), 1);
    iota(v2.begin(), v2.end(), MAX + 1);
    crear_asignar_threads(); 
    join_threads(threads);
    for (int i = 0; i < 100; i++)
    {
        printf("v3[%d]=%d\n",i, v3[i]);
    }
    
    return 0;
}
