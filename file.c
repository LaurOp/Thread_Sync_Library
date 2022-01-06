#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

// __sync_fetch_and_add(adresa, valoare)   --adunare atomica din stdlib
// __sync_fetch_and_sub(adresa, valoare)    --scaderea atomica din stdlib
// atomic_compare_exchange_weak(obiect_volatil, expected, dorit)    --compare and swap din stdatomic.h
// atomic_exchange(obiect_volatil, dorit)   --atribuire din stdatomic.h
// struct atomic_flag nume;     --FLAG lock-free atomic; nu accesez sau atribui valori ci folosesc:
// bool cond = atomic_flag_test_and_set(volatile atomic_flag* obj)    --seteaza flagu TRUE si intoarce valoarea precedenta
// void atomic_flag_clear(volatile atomic_flag* obj)     --seteaza flagu FALSE


// Bune doar daca folosesc tipuri de date atomice:
// int x = atomic_load(obiect_volatil) --ia valoarea dintr-o variabila atomica
// atomic_store(obiect_volatil, dorit) --pune valoarea intr-o variabila atomica

#define NR_THREADS 10

typedef struct{
    volatile atomic_flag* flag;  //flaguri de 'vreau acces' la zona critica
    int* tichet; //al catelea thread e in coada
    int thr_start;
}Mtx;

Mtx* Mtx_init(int nr_thr){      //CONSTRUCTOR
    Mtx* p = malloc(sizeof(Mtx));
    p->flag = malloc(nr_thr*sizeof(volatile atomic_flag));
    p->tichet = malloc(nr_thr*sizeof(int));
    p->thr_start = syscall(__NR_gettid);
    for (int i = 0; i < nr_thr; i++){
        atomic_flag_clear(&p->flag[i]);
        p->tichet[i] = 0;
    }
    return p;
}

void lock(Mtx* mt){    //MUTEX_LOCK
    
}

void unlock(Mtx* mt){
    mt->tichet[syscall(__NR_gettid) - mt->thr_start] = 0;
}

int main(){
    Mtx* m = Mtx_init(NR_THREADS);
    lock(m);
    unlock(m);
    return 0;
}