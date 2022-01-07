#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/syscall.h>
#include <unistd.h>

typedef struct{
    volatile _Atomic int owner;
} Mtx;

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

Mtx mut;
void * abc(void * p) {
  char * msg = (char *) p;
  int i;
  for (i = 0; i<100; i++){
    lock(&mut);
        for (size_t i = 0; i < strlen(msg); ++i) {
            printf("%c", msg[i]);
        }
        printf("\n");
    if(unlock(&mut)){
        printf("\neroare\n");
    }
  }
}

int main() {
    mtx_init(&mut);

    pthread_t aa;
    pthread_t bb;
    pthread_t cc;

    pthread_create(&aa, NULL, abc, "A");
    pthread_create(&bb, NULL, abc, "B");
    pthread_create(&cc, NULL, abc, "C");

    pthread_join(aa,NULL);
    pthread_join(bb,NULL);
    pthread_join(cc,NULL);

    mtx_destroy(&mut);
    return 0;
}
