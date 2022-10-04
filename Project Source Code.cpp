#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <queue>
#include <semaphore.h>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

// Variables to time(t) and be shared among threads
sem_t carSem;

// Initialize mutex variables
pthread_mutex_t lightMutex;
pthread_cond_t lightCondition;

// Keeps track of number of cars that have been created
int carCounter = 0;

// Direction that incoming cars are arriving from
string currentDirection = "north";

struct car {
    int id;
    char direction;
    struct timespec arrivalTime; // When they arrive
    struct timespec startTime;
    struct timespec endTime;
};

queue<car> nReadyQ;
queue<car> sReadyQ;

int pthread_sleep(int seconds) {
    pthread_mutex_t mutex;
    pthread_cond_t conditionvar;
    struct timespec timetoexpire;

    if (pthread_mutex_init(&mutex, NULL)) {
        return -1;
    }

    if (pthread_cond_init(&conditionvar, NULL)) {
        return -1;
    }

    timetoexpire.tv_sec = (unsigned int)time(NULL) + seconds;
    timetoexpire.tv_nsec = 0;
    return pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire);
}

// Produces random number of cars coming from the North direction
void* carNorth(void* args) {
    cout << "North thread" << endl;
    struct timespec arrival;
    struct car newCar;

    while (1) {
        sem_wait(&carSem);
        pthread_mutex_lock(&lightMutex);

        while ((rand() % 10) < 8) {
            cout << "At north locks" << endl;
            carCounter++;
            newCar.id = carCounter;
            newCar.direction = 'N';
            arrival.tv_sec = (unsigned int)time(NULL);
            arrival.tv_nsec = 0;
            newCar.arrivalTime = arrival;
            nReadyQ.push(newCar);
            pthread_sleep(1);
        }

        cout << "North side waits 60 seconds" << endl;
        pthread_sleep(60);
        pthread_cond_signal(&lightCondition);
        pthread_mutex_unlock(&lightMutex);
        sem_post(&carSem);
    }
    return 0;
}

// Produces random number of cars coming from the South direction
void* carSouth(void* args) {
    cout << "South thread " << endl;
    struct timespec arrival;
    struct car newCar;

    while (1) {
        sem_wait(&carSem);
        pthread_mutex_lock(&lightMutex);

        while ((rand() % 10) < 8) {
            cout << "At south locks" << endl;
            carCounter++;
            newCar.id = carCounter;
            newCar.direction = 'S';
            arrival.tv_sec = (unsigned int)time(NULL);
            arrival.tv_nsec = 0;
            newCar.arrivalTime = arrival;
            sReadyQ.push(newCar);
            pthread_sleep(1);
        }

        cout << "South side waits 60 seconds" << endl;
        pthread_sleep(60);
        pthread_cond_signal(&lightCondition);
        pthread_mutex_unlock(&lightMutex);
        sem_post(&carSem);
    }
    return 0;
}

// Changes the direction of traffic determined by light
void switchDirection() {
    if (currentDirection == "north") {
        currentDirection == "south";
    }
    else {
        currentDirection == "north";
    }
    return;
}

// Removes a car from queue, simulating the cars going through the tunnel
void processCar() {
    struct car processedCar;
    ofstream carLog;

    if (currentDirection == "north") {
        processedCar = nReadyQ.front();
        pthread_sleep(5);
        nReadyQ.pop();
    }
    else {
        processedCar = sReadyQ.front();
        pthread_sleep(5);
        sReadyQ.pop();
    }

    cout << left << setw(12) << processedCar.id << processedCar.direction
        << endl;
    carLog.open("car.log", ios_base::app);
    carLog << left << setw(12) << processedCar.id << processedCar.direction
        << "\n";
    carLog.close();

    return;
}

// Determines whether light is GREEN or RED
void switchLight() {
    ofstream lightLog;

    while (!nReadyQ.empty() && !sReadyQ.empty()) {
        cout << left << setw(12) << time(NULL) << "sleep" << endl;

        lightLog.open("light.log", ios_base::app);
        lightLog << left << setw(12) << time(NULL) << "sleep\n";
        lightLog.close();
        pthread_sleep(1);
        pthread_cond_wait(&lightCondition, &lightMutex);
    }

    cout << left << setw(12) << time(NULL) << "woken-up" << endl;
    lightLog.open("light.log", ios_base::app);
    lightLog << left << setw(12) << time(NULL) << "woken-up\n";
    lightLog.close();
}

// Car consumer thread
void* passThroughTunnel(void* args) {
    while (1) {
        pthread_mutex_lock(&lightMutex);

        cout << "Side that is going through: " << currentDirection << endl;

        if (currentDirection == "north") {
            if ((sReadyQ.size() >= 10) && (nReadyQ.size() < 10)) {
                switchDirection();
            }
            else if (nReadyQ.empty() && sReadyQ.size() >= 10) {
                switchDirection();
            }
            else if (nReadyQ.empty()) {
                switchLight();
            }
            else {
                processCar();
            }
        }
        else {
            if (nReadyQ.size() >= 10 && sReadyQ.size() < 10) {
                switchDirection();
            }
            else if (sReadyQ.empty() && nReadyQ.size() >= 10) {
                switchDirection();
            }
            else if (sReadyQ.empty()) {
                switchLight();
            }
            else {
                processCar();
            }
        }

        pthread_mutex_unlock(&lightMutex);
    }

    return 0;
}

int main() {
    pthread_t sWay, nWay, fWay;     // Create three threads South, North, and Final
    int pshared = 1;
    int semValue = 1;
    srand(time(NULL));

    // Mutex is initialized
    if (pthread_mutex_init(&lightMutex, NULL)) {
        perror("mutex_init");
        return -1;
    }

    // Conditional var is initialized
    if (pthread_cond_init(&lightCondition, NULL)) {
        perror("cond_init");
        return -1;
    }

    // Initialize semaphore
    if (0 != sem_init(&carSem, pshared, semValue)) {
        perror("sem_init");
        return -1;
    }

    // Creating 3 threads 

    // Create thread for car generator in the North direction
    if (-1 == pthread_create(&nWay, NULL, carNorth, NULL))
        return -1;

    // Create thread for car generator in the South direction
    if (-1 == pthread_create(&sWay, NULL, carSouth, NULL))
        return -1;

    // Create thread for tunnel that passes cars
    if (-1 == pthread_create(&fWay, NULL, passThroughTunnel, NULL))
        return -1;

    while (1) {
        fflush(stdout);
        pthread_sleep(1);
    }

    sem_close(&carSem);
    pthread_mutex_destroy(&lightMutex);
    pthread_cond_destroy(&lightCondition);

    return 0;
}
