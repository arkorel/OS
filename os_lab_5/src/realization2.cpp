#include "realizations.h"

 float Pi(int K){
    float del = 2;
    float div = 1;
    int count = 1;
    float pi1 = 1;
    for (int i = 1; i<=K; i++){
        pi1 = pi1*del/div;
        if(count % 2 == 1) div = div + 2;
        else del = del+2;
        ++count;
    }
    float pi = 2*pi1;
    return pi;
}


int* Sort(int* s_arr, int last, int first){  
  int i = first, j = last, x = s_arr[(first + last) / 2];  
    do {
        while (s_arr[i] < x) i++;
        while (s_arr[j] > x) j--;
  
        if(i <= j) {
            if (s_arr[i] > s_arr[j]){
              int t;
              t = s_arr[i];
              s_arr[i] = s_arr[j];
              s_arr[j] = t;
            }
            i++;
            j--;
        }
    } while (i <= j);
  
    if (i < last)
        Sort(s_arr, last, i);
    if (first < j)
        Sort(s_arr, j, first);
}
