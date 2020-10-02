#ifndef IZ2_LIB_DEF_H
#define IZ2_LIB_DEF_H

#include <stdio.h>
#define __USE_GNU ;
#define __GNU_SOURCE ;
#include <pthread.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#define SUCCESS 1;
#define FAILED (-1);


typedef struct{
    int size;
    int offset;
    int cnt;
    char* string;
    FILE* file;
}thread_data;

#endif //IZ2_LIB_DEF_H
