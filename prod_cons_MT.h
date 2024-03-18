#ifndef PROD_CONS_MT_H
#define PROD_CONS_MT_H

#include <pthread.h>
#include <vector>
extern int activeProducers;
extern pthread_mutex_t activeProducersMutex;
class Monitor {
public:
  Monitor(int size);
  ~Monitor();

  // Methods to manage item production and consumption
  void produce(int item, int id); // Includes thread ID for logging
  int consume(int id);            // Includes thread ID for logging
  bool isEmpty() const;
  // Active producer management methods
  void incrementActiveProducers();
  void decrementActiveProducers();
  bool areProducersActive();
void finalCheckAndBroadcast() {
    pthread_mutex_lock(&mutex);
    if (activeProducers == 0 && buffer.empty()) {
        pthread_cond_broadcast(&cond_consume);
    }
    pthread_mutex_unlock(&mutex);
}

private:
  std::vector<int> buffer;     // Shared buffer
std::vector<int>::size_type bufferSize;              // Maximum size of the buffer
  pthread_mutex_t mutex;       // Mutex for synchronization
  pthread_cond_t cond_produce; // Condition variable for producers
  pthread_cond_t cond_consume; //
  bool isEmpty();
};

// Structure to pass data to producer and consumer threads
struct ThreadData {
  Monitor *monitor; // Pointer to the shared Monitor instance
  int count;        // Number of items to produce/consume
  int id;           // Thread identifier for logging purposes
};

// Function prototypes for producer and consumer threads
void *producer(void *param);
void *consumer(void *param);

#endif // PROD_CONS_MT_H
