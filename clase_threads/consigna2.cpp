#include <mutex>       // std::mutex
#include <thread>      // std::thread
#include <semaphore.h> // POSIX semaphores
#include <atomic>      // Atomic elements (std C++11)
#include <iostream>
#include <vector>
#include <functional>
#include <unistd.h>

#define MSG_COUNT 5

using namespace std;

sem_t semA;
sem_t semB;
vector<thread> threads(2);

void f1_a() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F1 (A)\n";
        this_thread::sleep_for(100ms);
    }
}

void f1_b() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F1 (B)\n";
        this_thread::sleep_for(100ms);
    }
}

void f2_a() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F2 (A)\n";
        this_thread::sleep_for(100ms);
    }
}

void f2_b() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F2 (B)\n";
        this_thread::sleep_for(100ms);
    }
}

// primer post y despues wait. VER
void f1() {
    f1_a();
    sem_post(&semA);
    sem_wait(&semB);
    f1_b();
}

void f2() {
    f2_a();
    sem_wait(&semA);
    sem_post(&semB);
    f2_b();
}

void crear_asignar_threads(vector<thread> &threads){
    threads[0] = thread(f1);
    threads[1] = thread(f2);
}

// Hago join de los threads, de esta forma es1
void join_threads(vector<thread> &threads){
    for (auto &t : threads) { 
        t.join();
    }
}
int main() {
    sem_init(&semA, 0, 0);
    sem_init(&semB, 0, 0);
    crear_asignar_threads(threads); 
    join_threads(threads);
    return 0;
}
