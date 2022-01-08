#include "sinc.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/syscall.h>
#include <unistd.h>

//                                   SPINLOCK
//  ===================================================================================

void spn_init(Spn* Mut){
    Mut->owner = 0;
}

void spn_lock(Spn* Mut){
    while (!__sync_bool_compare_and_swap(&Mut->owner, 0, syscall(__NR_gettid))){};
}   

int spn_unlock(Spn* Mut) {
    if (Mut->owner == 0){
        atomic_store(&Mut->owner, 0);
        return -1;  //EROARE UNLOCK LA CEVA UNLOCKED
    }
    //while (!__sync_bool_compare_and_swap(&Mut->owner, syscall(__NR_gettid), 0)){};
    if (Mut->owner != syscall(__NR_gettid)){
        atomic_store(&Mut->owner, 0);
        return -2;  //EROARE OWNERSHIP
    }
    atomic_store(&Mut->owner, 0);
    return 0;
}

int spn_destroy(Spn* Mut){
    if (Mut->owner){
        printf("eroare in destroyer");
        return -1;
    }
    return 0;
}



//                                  SEMAFOR BINAR
//  ====================================================================================

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



//                                     RWLOCK
//  ====================================================================================

void rwl_init(Rwlock* rwl)
{
    rwl->counter = 0;
    sem_binar_init(&rwl->counter_lock);
    spn_init(&rwl->write_lock);
}

void lock_for_reading(Rwlock* rwl)
{   
    lock_sb(&rwl->counter_lock);
    rwl->counter++;
    if(rwl->counter == 1)
        spn_lock(&rwl->write_lock);
    unlock_sb(&rwl->counter_lock);
}

void unlock_for_reading(Rwlock* rwl)
{   
    lock_sb(&rwl->counter_lock);
    rwl->counter--;
    if(rwl->counter == 0)
        spn_unlock(&rwl->write_lock);
    unlock_sb(&rwl->counter_lock);
}

void lock_for_writing(Rwlock* rwl)
{
    spn_lock(&rwl->write_lock);
}

void unlock_for_writing(Rwlock* rwl)
{   
    spn_unlock(&rwl->write_lock);
}

void rwl_destroy(Rwlock* rwl)
{
    sem_binar_destroy(&rwl->counter_lock);
    spn_destroy(&rwl->write_lock);
}



//                                    SEMAFOR
//  ====================================================================================


