#ifndef SINC_H_
#define SINC_H_
#include <stdatomic.h>

typedef struct{
    volatile _Atomic int owner;
} Mtx;

void mtx_init(Mtx* Mut);
void lock(Mtx* Mut);
int unlock(Mtx* Mut);
void mtx_destroy(Mtx* Mut);

typedef struct{
    volatile atomic_flag flg;
} Sem_binar;        //UN MUTEX CARE NU VERIFICA OWNERSHIP

void sem_binar_init(Sem_binar* sb);
void lock_sb(Sem_binar* sb);
int unlock_sb(Sem_binar* sb);
void sem_binar_destroy(Sem_binar* sb);


typedef struct{
    Mtx write_lock;
    Sem_binar counter_lock;
    int counter;
} Rwlock;

void rwl_init(Rwlock* rwl);
void lock_for_reading(Rwlock* rwl);
void unlock_for_reading(Rwlock* rwl);
void lock_for_writing(Rwlock* rwl);
void unlock_for_writing(Rwlock* rwl);
void rwl_destroy(Rwlock* rwl);

#endif
