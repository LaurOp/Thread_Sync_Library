#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/syscall.h>
#include <unistd.h>

// #define mut_type volatile atomic_flag

typedef struct{
    volatile atomic_flag mut;
    volatile int owner;
} Mtx;

void mtx_init(Mtx* Mut){
    atomic_flag_clear(&Mut->mut);
    Mut->owner = 0;
}

void lock(Mtx* Mut){
    atomic_store(&Mut->owner, syscall(__NR_gettid));
    while (atomic_flag_test_and_set(&Mut->mut));      //daca e fals, lasa-l sa treaca, altfel astepti unlock  
}   
int unlock(Mtx* Mut) {
    atomic_flag_clear(&Mut->mut);
    return 0;
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

    pthread_create(&aa, NULL, abc, "AAA");
    pthread_create(&bb, NULL, abc, "BBB");
    pthread_create(&cc, NULL, abc, "CCC");

    pthread_join(aa,NULL);
    pthread_join(bb,NULL);
    pthread_join(cc,NULL);

    return 0;
} // main
