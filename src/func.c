#include "func.h"


char* input_str(FILE* f){
    int counter = 0;
    int size_a = 0;
    char* string = NULL;
    char c = getc(f);
    while(c != EOF && c!= '\n'){
        if(counter >= size_a){
            char* tmp_string = malloc(((!size_a ? 2:size_a*2+1)*sizeof(char)));
            size_a = (!size_a ? 1: size_a*2);
            if(!tmp_string){
                if(string)
                    free(string);
                return NULL;
            }
            if(string){
                strcpy(tmp_string,string);
                free(string);
            }
            string = tmp_string;
        }
        string[counter++] = c;
        string[counter] = '\0';
        c = getc(f);
    }
    return string;
}

long time_sec(){
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    long mt = (long)t.tv_sec * 1000 + t.tv_nsec / 1000000;
    return mt;
}

