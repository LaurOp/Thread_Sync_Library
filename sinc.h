#ifndef SINC_H_
#define SINC_H_

typedef struct{
    volatile _Atomic int owner;
} Mtx;

void mtx_init(Mtx* Mut);
void lock(Mtx* Mut);
int unlock(Mtx* Mut);
void mtx_destroy(Mtx* Mut);


#endif
