
#ifndef IZ2_LIBD_HDR_H
#define IZ2_LIBD_HDR_H

#include <stdio.h>
#define __USE_GNU ;
#define __GNU_SOURCE ;
#include <pthread.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#define SUCCESS 1;
#define FAIL -1;


typedef struct{
    int size;
    int offset;
    int cnt;
    char* string;
    FILE* file;
}thread_data;


int count_entries(FILE* file,char* filename, char* string);
int create_threads(int procs_cnt,thread_data* data);
void* cnt_thr_entries(void* thr_data);
int GetFileSize(char* f);
int GetStringSize(char* string);
thread_data* create_data(char* f,int procs_cnt, char* string,int file_size,int string_size);
int free_threads(pthread_t* thread);
int free_threads_data(thread_data* data, int size);
#endif
