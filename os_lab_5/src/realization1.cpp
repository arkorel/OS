#include "realizations.h"

 float Pi(int K){
    float pi = 0;
    float del = 1;
    for (int i = 1; i<=K; i++){
        if(i % 2 == 1) pi = pi + 4/del;
        else pi = pi-4/del;
        del = del+2;
    }
    return pi;
}

 int * Sort(int * array, int size, int left){
    ++size;
    int tmp;
        for (int i = 0; i < size; i++) {
        for (int j = 0; j < size - 1; j++) {
            if (array[j] > array[j + 1]) {
                tmp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = tmp;
            }
        }
    }
}
