#include "prod_cons_MT.h"
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

using namespace std;

int activeProducers = 0;
pthread_mutex_t activeProducersMutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <buffer_size> <num_producers> <num_consumers>\n";
        return 1;
    }

    srand(static_cast<unsigned>(time(nullptr))); // Seed for random number generation

    int bufferSize = atoi(argv[1]);
    int numProducers = atoi(argv[2]);
    int numConsumers = atoi(argv[3]);
    int itemsPerProducer = 6; // Adjust as needed, based on the expected workload

    Monitor monitor(bufferSize);
    vector<pthread_t> producers(numProducers), consumers(numConsumers);
    vector<ThreadData> pData(numProducers), cData(numConsumers);

    // Create producer threads
    for (int i = 0; i < numProducers; ++i) {
        pData[i] = {&monitor, itemsPerProducer, i};
        if (pthread_create(&producers[i], nullptr, producer, &pData[i])) {
            cout << "Failed to create producer thread " << i << endl;
            return 1;
        }
        cout << "Main: started producer " << i << endl;
    }


    for (int i = 0; i < numConsumers; ++i) {
        cData[i] = {&monitor, itemsPerProducer, i}; // Adjust if consumers should consume differently
        if (pthread_create(&consumers[i], nullptr, consumer, &cData[i])) {
            cout << "Failed to create consumer thread " << i << endl;
            return 1;
        }
        cout << "Main: started consumer " << i << endl;
    }

    // Join producer threads
    for (int i = 0; i < numProducers; ++i) {
        if (pthread_join(producers[i], nullptr)) {
            cout << "Failed to join producer thread " << i << endl;
        }
        cout << "Main: producer " << i << " joined\n";
    }

    // Join consumer threads
    for (int i = 0; i < numConsumers; ++i) {
        if (pthread_join(consumers[i], nullptr)) {
            cout << "Failed to join consumer thread " << i << endl;
        }
        cout << "Main: consumer " << i << " joined\n";
    }

    cout << "Main: program complete\n";

    return 0;
}
