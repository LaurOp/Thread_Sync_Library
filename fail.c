#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>

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

#define NR_THREADS 100

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
    volatile int thr_actual;
    atomic_exchange (&thr_actual, syscall(__NR_gettid) - mt->thr_start - 1);
    
    atomic_flag_test_and_set(&mt->flag[thr_actual]);
    
    int max_tichete = 0;
    int i;
    for (i = 0; i < NR_THREADS; i++){
        int acum = atomic_load(&mt->tichet[i]);

        if (acum > max_tichete)
            max_tichete = acum;
    }
    max_tichete += 1;
    
    atomic_exchange(&mt->tichet[thr_actual], max_tichete);
    
    atomic_flag_clear(&mt->flag[thr_actual]);

    for(i = 0; i < NR_THREADS; i++){
        if (i != thr_actual){
            }
    }

}

void unlock(Mtx* mt){   //MUTEX_UNLOCK care doar seteaza flagu de 'intrat' la false
    atomic_exchange(&mt->tichet[syscall(__NR_gettid) - mt->thr_start], 0);
    }

//IMPLEMENT DESTROYER!

Mtx* m;
int x;
void* thread_routine(void* arg){
    int param = *(int *)arg;
    
    lock(m);
    x += 1;
    x -= 1;
    x += 1;
    x -= 1;
    x += 1;
    unlock(m);
}

int main(){
    m = Mtx_init(NR_THREADS);

    pthread_t thread[NR_THREADS];
    int* arg = malloc(NR_THREADS*sizeof(int));
    int i;
    for(i = 0; i<NR_THREADS; i++)
        arg[i] = i+1;
    for(i = 0; i<NR_THREADS; i++){
        pthread_create(&thread[i], NULL, thread_routine, &arg[i]);
    }
    for(i = 0; i<NR_THREADS; i++){
        pthread_join(thread[i], NULL);
    }
    printf("%d ",x);
    return 0;
}
