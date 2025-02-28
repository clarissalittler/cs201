#include <semaphore.h>
struct sharedData {
  sem_t mutex;
  int payload1;
  int payload2;
};

#define sharedName "/shared_data_mem"
