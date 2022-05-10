#include <iostream>
//#include "realizations.h"

extern "C" float Pi(int K);
extern "C" int * Sort(int * array, int left, int right = 0);

int main(){
    int command;
    std::cout << "Insert a command\n 1 - engage Pi function\n 2 - engage Sort function\n";
    while(scanf("%d", &command) != EOF){
        //std::cin >> command;
        switch(command){
            case 1:{ //pi function
                std::cout << "Insert accuracy K\n";
                int K;
                std::cin >> K;
                float pi = Pi(K);
                std::cout << "Result is: " << pi << "\n";
                break;
            }
            case 2:{ //sort function
                std::cout << "Insert array length\n";
                int N;
                std::cin >> N;
                int array[N];
                std::cout << "Insert array elements\n";
                for(int i = 0; i<N; ++i){
                    std::cin >> array[i];
                };
                int size = sizeof(array)/sizeof(array[0]);
                Sort(array, size-1, 0);
                std::cout << "Result is:\n";
                for(int i = 0; i<N; ++i){
                    std::cout << array[i] << " ";
                }
                std::cout << "\n";
                break;
            }
                default:{
                std::cout << "--Wrong command!--\n";
                std::cout << "Insert a command\n 0 - change library\n 1 - engage Pi function\n 2 - engage Sort function\n";
            }
        }   
    }
    std::cout << "Shutting down...\n";

}
