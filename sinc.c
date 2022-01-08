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



void sem_binar_init(Sem_binar* sb){
    atomic_flag_clear(&sb->flg);
}

void lock_sb(Sem_binar* sb){
    while (atomic_flag_test_and_set(&sb->flg));
}

int unlock_sb(Sem_binar* sb){
    atomic_flag_clear(&sb->flg);
}

void sem_binar_destroy(Sem_binar* sb){
    atomic_flag_clear(&sb->flg);
}



void rwl_init(Rwlock* rwl){
    rwl->counter = 0;
    sem_binar_init(&rwl->counter_lock);
    mtx_init(&rwl->write_lock);
}

void lock_for_reading(Rwlock* rwl)
{   
    lock_sb(&rwl->counter_lock);
    rwl->counter++;
    if(rwl->counter == 1)
        lock(&rwl->write_lock);
    unlock_sb(&rwl->counter_lock);
}

void unlock_for_reading(Rwlock* rwl)
{   
    lock_sb(&rwl->counter_lock);
    rwl->counter--;
    if(rwl->counter == 0)
        unlock(&rwl->write_lock);
    unlock_sb(&rwl->counter_lock);
}

void lock_for_writing(Rwlock* rwl)
{
    lock(&rwl->write_lock);
}

void unlock_for_writing(Rwlock* rwl)
{   
    unlock(&rwl->write_lock);
    sem_binar_destroy(&rwl->counter_lock);
    mtx_destroy(&rwl->write_lock);
}
