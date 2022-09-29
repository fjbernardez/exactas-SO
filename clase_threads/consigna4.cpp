#include <mutex>       // std::mutex
#include <thread>      // std::thread
#include <semaphore.h> // POSIX semaphores
#include <atomic>      // Atomic elements (std C++11)
#include <iostream>
#include <vector>
#include <functional>
#include <unistd.h>
#include <numeric>

#define MAX     100
#define THREADS 5

using namespace std;

vector<int> v1(MAX);
vector<int> v2(MAX);
vector<int> v3(MAX);
vector<int> operaciones(THREADS);

vector<thread> threads(THREADS);

atomic<int> index {0};

void multiplicar(int thread_number) {
    int index_aux;
    index_aux = index.fetch_add(1);
    while (index_aux < MAX) {
        v3[index_aux] = v1[index_aux] * v2 [index_aux];
        operaciones[thread_number]++;
        this_thread::sleep_for(100ms*(thread_number+1));
        index_aux = index.fetch_add(1);
    }   
}

void crear_asignar_threads() {
    threads[0] = thread(multiplicar, 0);
    threads[1] = thread(multiplicar, 1);
    threads[2] = thread(multiplicar, 2);
    threads[3] = thread(multiplicar, 3);
    threads[4] = thread(multiplicar, 4);
}

// Hago join de los threads, de esta forma es1
void join_threads(vector<thread> &threads){
    for (auto &t : threads) { 
        t.join();
    }
}
int main() {
    iota(v1.begin(), v1.end(), 1);
    iota(v2.begin(), v2.end(), MAX + 1);
    v3.assign(MAX, 25);
    operaciones.assign(THREADS, 0);
    for (int i = 0; i < THREADS; i++) {
        printf("operaciones[%d]=%d\n", i, operaciones[i]);
    }
    
    crear_asignar_threads(); 
    join_threads(threads);
    for (int i = 0; i < 100; i++) {
        printf("v3[%d]=%d \tdeberia ser igual a v1[%d]=%d * v2[%d]=%d\n",i, v3[i],i, v1[i],i, v2[i]);
    }
    
    printf("\n==========\n");

    for (int i = 0; i < THREADS; i++) {
        printf("operaciones[%d]=%d\n", i, operaciones[i]);
    }
    
    return 0;
}
