#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "sinc.h"

int nr = 0;
Rwlock rwl;

void* readt(){
    lock_for_reading(&rwl);
    printf("Din thread de read %d, iar counter-ul este %d\n", nr, rwl.counter);
    unlock_for_reading(&rwl);

    return NULL;
}

void* writet(){
    lock_for_writing(&rwl);
    nr ++;
    printf("Din thread de write %d, iar counter-ul este %d\n", nr, rwl.counter);
    unlock_for_writing(&rwl);

    return NULL;
}

#define NR_THREADS 50
int main()
{
    pthread_t threads[NR_THREADS];
    rwl_init(&rwl);

    for(int i = 0; i < NR_THREADS; i++){
        if(i%2)
            pthread_create(&threads[i], NULL, readt, NULL);
        else
            pthread_create(&threads[i], NULL, writet, NULL);
    }

    for(int i = 0; i < NR_THREADS; i++){
        pthread_join(threads[i],NULL);
    }

    return 0;
}
