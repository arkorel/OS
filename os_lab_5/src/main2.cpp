#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <dlfcn.h>

int main(){
    int command = 1;
    int libmode = 1; 
    void *handle; 
    handle = dlopen("libdynamic1.so",RTLD_LAZY);
    if (!handle){
        fprintf(stderr, "dlopen() error: %s\n", dlerror());
        exit(1);
    }
    float (*Pi)(int);
    int* (*Sort)(int*, int, int);
    Pi = (float(*)(int))dlsym(handle, "Pi");
    Sort = (int*(*)(int*,int,int))dlsym(handle, "Sort");
    //std::cout << handle << "\n";
    std::cout << "Now using lib1\n";
    std::cout << "Insert a command\n 0 - change library\n 1 - engage Pi function\n 2 - engage Sort function\n";
    while(scanf("%d", &command) != EOF){
        //std::cin >> command;
        switch(command){
            case 0:{ //change lib
                dlclose(handle);
                if(libmode == 1){
                    handle = dlopen("libdynamic2.so", RTLD_LAZY);
                if (!handle){
                fprintf(stderr, "dlopen() error: %s\n", dlerror());
                exit(1);
                }
                    libmode = 2;
                    std::cout << "Now using lib2\n";
                } else if(libmode == 2){
                if (!handle)
                {
                fprintf(stderr, "dlopen() error: %s\n", dlerror());
                exit(1);
                }
                    handle = dlopen("libdynamic1.so", RTLD_LAZY);
                    libmode = 1;
                    std::cout << "Now using lib1\n";
                }
                Pi = (float(*)(int))dlsym(handle, "Pi");
                Sort = (int*(*)(int*,int,int))dlsym(handle, "Sort");
                break;
            }
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
    dlclose(handle);
    std::cout << "Library closed.\n";

}
