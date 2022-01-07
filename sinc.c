#include "sinc.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/syscall.h>
#include <unistd.h>

void mtx_init(Mtx* Mut){
    Mut->owner = 0;
}

void lock(Mtx* Mut){
    while (!__sync_bool_compare_and_swap(&Mut->owner, 0, syscall(__NR_gettid)));
}   

int unlock(Mtx* Mut) {
    if (Mut->owner == 0){
        return -1;  //EROARE UNLOCK LA CEVA UNLOCKED
    }
    if (Mut->owner != syscall(__NR_gettid) ){
        return -2;  //EROARE OWNERSHIP
    }
    atomic_store(&Mut->owner, 0);
    return 0;
}

void mtx_destroy(Mtx* Mut){
}
