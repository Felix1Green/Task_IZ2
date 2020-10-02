#include <stdio.h>
#include <dlfcn.h>
#include "lib_def.h"
#include "func.h"


int main(int argc, char* argv[]) {
    // -----------------dynamic_library include-----------------------
    void * library = dlopen(PATH_TO_DYN_LIB, RTLD_LAZY);
    if(!library){
        puts("library open error\n");
        dlclose(library);
        return FAILED;
    }


    int (*GetFileSize)(char* f);
    int (*GetStringSize)(char* string);
    thread_data* (*create_data)(char* f,int procs_cnt, char* string,int file_size,int string_size);
    int (*create_threads)(int procs_cnt,thread_data* data);
    int (*free_threads_data)(thread_data* data, int size);

    *(void**) (&GetFileSize) = dlsym(library,"GetFileSize");
    if(!GetFileSize){
        fprintf(stderr,"%s\n",dlerror());
        dlclose(library);
        return FAILED;
    }
    *(void**)(&GetStringSize) = dlsym(library,"GetStringSize");
    if(!GetStringSize){
        fprintf(stderr,"%s\n",dlerror());
        dlclose(library);
        return FAILED;
    }
    *(void**)(&create_data) = dlsym(library,"create_data");
    if(!create_data){
        fprintf(stderr,"%s\n",dlerror());
        dlclose(library);
        return FAILED;
    }
    *(void**)(&create_threads) = dlsym(library,"create_threads");
    if(!create_threads){
        fprintf(stderr,"%s\n",dlerror());
        dlclose(library);
        return FAILED;
    }
    *(void**)(&free_threads_data) = dlsym(library,"free_threads_data");
    if(!free_threads_data){
        fprintf(stderr,"%s\n",dlerror());
        dlclose(library);
        return FAILED;
    }
    //-----------------------------------------------------------------------
    FILE* test = NULL;
    test = fopen(PATH_TO_FILE,"r");
    if(!test) {
        printf("FILE_ERROR\n");
        return FAILED;
    }
    printf("Enter the required string\n");
    char* string = input_str(stdin);
    if(!string) {
        fclose(test);
        return (puts("REQUIRED STRING ERROR"));
    }

    long start = time_sec();
    int procs_cnt = get_nprocs();
    int file_size = GetFileSize(PATH_TO_FILE);
    int string_size = GetStringSize(string);
    if(string_size > file_size) {
        free(string);
        fclose(test);
        return FAILED;
    }
    thread_data* data = create_data(PATH_TO_FILE,procs_cnt,string,file_size,string_size);
    if(!data) {
        free(string);
        fclose(test);
        return FAILED;
    }
    if(!create_threads(procs_cnt,data)) {
        fclose(test);
        free(string);
        free_threads_data(data,procs_cnt);
        return FAILED;
    }
    int result = 0;
    for(int i =0; i < procs_cnt;i++){
        result += data[i].cnt;
    }
    long end = time_sec();
    free(string);
    free_threads_data(data,procs_cnt);
    dlclose(library);
    printf("result: %d\n",result);
    printf("Dynamic library spends %ld ms\n",end-start);
    fclose(test);
    return result;
}
