#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>

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


int main(){
    int* a = malloc(sizeof(int));
    int* b = a;
    *a = 5;
    printf("%d",*a);
}