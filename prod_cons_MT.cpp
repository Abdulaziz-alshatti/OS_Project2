#include "prod_cons_MT.h"
#include <iostream>
#include <unistd.h> 

using namespace std;

extern int activeProducers;
extern pthread_mutex_t activeProducersMutex;

Monitor::Monitor(int size) : bufferSize(size) {
  pthread_mutex_init(&mutex, nullptr);
  pthread_cond_init(&cond_produce, nullptr);
  pthread_cond_init(&cond_consume, nullptr);
}

Monitor::~Monitor() {
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond_produce);
  pthread_cond_destroy(&cond_consume);
}

void Monitor::produce(int item, int id) {
  pthread_mutex_lock(&mutex);
  while (buffer.size() >= bufferSize) {
    cout << "P" << id << ": Blocked due to full buffer\n";
    pthread_cond_wait(&cond_produce, &mutex);
  }

  buffer.push_back(item);
  cout << "P" << id << ": Writing " << item << " to position "
            << buffer.size() - 1 << endl;
  pthread_cond_signal(&cond_consume);
  pthread_mutex_unlock(&mutex);
}

int Monitor::consume(int id) {
  pthread_mutex_lock(&mutex);
  while (buffer.empty()) {
    if (!areProducersActive()) {
      pthread_mutex_unlock(&mutex);
      return -1; // Indicate no more items will be produced
    }
    cout << "C" << id << ": Blocked due to empty buffer\n";
    pthread_cond_wait(&cond_consume, &mutex);
  }

  int item = buffer.front();
  buffer.erase(buffer.begin());
  cout << "C" << id << ": Reading " << item << " from position 0\n";
  pthread_cond_signal(&cond_produce);
  pthread_mutex_unlock(&mutex);
  return item;
}

bool Monitor::areProducersActive() {
  pthread_mutex_lock(&activeProducersMutex);
  bool active = activeProducers > 0;
  pthread_mutex_unlock(&activeProducersMutex);
  return active;
}

bool Monitor::isEmpty() const { return buffer.empty(); }

void *producer(void *param) {
  auto *data = static_cast<ThreadData *>(param);

  pthread_mutex_lock(&activeProducersMutex);
  activeProducers++;
  pthread_mutex_unlock(&activeProducersMutex);

  cout << "P" << data->id << ": Producing " << data->count << " values\n";
  for (int i = 0; i < data->count; ++i) {
    int item = rand() % 10 + 1;
    data->monitor->produce(item, data->id);
  }

  pthread_mutex_lock(&activeProducersMutex);
  activeProducers--;
  bool lastProducer = activeProducers == 0;
  pthread_mutex_unlock(&activeProducersMutex);

  if (lastProducer) {
    data->monitor->finalCheckAndBroadcast();
  }

  return nullptr;
}

void *consumer(void *param) {
  auto *data = static_cast<ThreadData *>(param);
  while (true) {
    int result = data->monitor->consume(data->id);
    if (result == -1 && !data->monitor->areProducersActive()) {
      break; // Exit loop if consume indicates no more items and no producers
             // are active
    }
  }
  cout << "C" << data->id << ": Exiting\n";
  return nullptr;
}
