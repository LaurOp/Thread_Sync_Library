#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sinc.h"

pthread_t tid[2];
int counter;
Mtx loc;
  
void* trythis(void* arg)
{
    lock(&loc);
  
    unsigned long i = 0;
    counter += 1;
    printf("Job %d has started\n", counter);
  
    for (i = 0; i < 100000; i++)
        ;
  
    printf("Job %d has finished\n\n", counter);
  
    unlock(&loc);
  
    return NULL;
}
  
int main(void)
{
    int i = 0;
    int error;
  
    mtx_init(&loc);
  
    while (i < 100) {
        error = pthread_create(&(tid[i]),
                               NULL,
                               &trythis, NULL);
        if (error != 0)
            printf("\nThread can't be created :[%s]",
                   strerror(error));
        i++;
    }
  
    for(int j = 0; j< 100;j++)
        pthread_join(tid[j], NULL);

    mtx_destroy(&loc);
  
    return 0;
}
