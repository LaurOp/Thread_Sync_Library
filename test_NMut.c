
#include "pthread.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "stdlib.h"
#include <sys/syscall.h> 
#include "sinc.h"

#define NR_THREADS 100
volatile int resource = 0;

NMut m;

void* thread_body(void* arg)
{
    long thread = (long)arg;
    nmut_lock(&m);
    if (resource != 0) {
        printf("Resource was acquired by %d, but is still in-use by %d!\n",
               thread, resource);
    }
  
    resource = thread;
    printf("%d using resource...\n", thread);
  
    resource = 0;
    nmut_unlock(&m);
    return NULL;
}
  
int main(int argc, char** argv)
{
    nmut_init(&m,NR_THREADS);

    pthread_t threads[NR_THREADS];
  
    for (int i = 0; i < NR_THREADS; ++i) {
        pthread_create(&threads[i], NULL, &thread_body, (void*)((long)i));
    }
  
    for (int i = 0; i < NR_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }
    nmut_destroy(&m);
    return 0;
}