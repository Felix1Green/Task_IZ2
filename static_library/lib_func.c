#include "lib_hdr.h"


int count_entries(FILE* f,const char* string){
    if(!f || !string)
        return FAILED;
    char c;
    int cnt = 0;int index = 0;
    while(!feof(f)){
        c = getc(f);
        if(string[index] == '\0')
            index=0,cnt++;
        if(c != string[index])
            index = 0;
        if(c == string[index]){
            index++;
        }
    }
    return cnt;
}