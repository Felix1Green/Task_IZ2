#include "libd_hdr.h"

// инициализация данных для каждого потока
thread_data* create_data(char* f, int procs_cnt,char* string,int file_size,int string_size){
    if(!f || !string)
        return NULL;
    int file_pr_thr_size = file_size/procs_cnt;
    thread_data* info = (thread_data*)malloc(procs_cnt*sizeof(thread_data));
    if(!info)
        return NULL;
    // заполнение необходимых данных для каждого потока
    for(int i =0; i < procs_cnt; i++){
        info[i].size = file_pr_thr_size;
        info[i].offset = 0;
        //организация смещения курсора файла для каждого потока
        if(i > 0){
            if(i == procs_cnt-1)
                info[i].size = file_size - (procs_cnt-1)*file_pr_thr_size;
            info[i].size += string_size;
            info[i].offset = info[i-1].offset + info[i-1].size - string_size;
            if(info[i].offset < 0)
                info[i].offset = 0;
        }
        info[i].file = fopen(f,"r");
        if(!info[i].file) {
            free_threads_data(info,procs_cnt);
            return NULL;
        }
        // перемещение курсора
        fseek(info[i].file, info[i].offset, SEEK_SET);
        info[i].string = string;
        info[i].cnt = 0;
    }
    return info;
}

//функция подсчета вхождений
void* cnt_thr_entries(void* thr_data){
    thread_data* data = (thread_data*)thr_data;
    char c;
    int sz = 0;
    int index = 0;
    int cnt = 0;
    const char*string = data->string;
    while(sz < data->size){
        c = getc(data->file);
        if(string[index] == '\0')
            index=0,cnt++;
        if(c != string[index])
            index = 0;
        if(c == string[index]){
            index++;
        }
        sz++;
    }
    if(string[index] == '\0')
        cnt++;
    data->cnt = cnt;
    return NULL;
}


// инициализация потоков
int create_threads(int procs_cnt,thread_data* data){
    if(!data){
        return FAIL;
    }
    pthread_t* thread = (pthread_t*)malloc(procs_cnt*sizeof(pthread_t));
    if(!thread)
        return FAIL;
    int result = SUCCESS;
    for(size_t i = 0; i < procs_cnt; i++){
        int s = pthread_create(thread+i,NULL,cnt_thr_entries,data+i);
        if(s) {
            for (int j = 0; j < i; j++)
                pthread_join(thread[j], NULL);
            result = FAIL;
        }
        //set_affinity
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(i, &cpuset);
        int rc = pthread_setaffinity_np(thread[i],sizeof(cpu_set_t),&cpuset);
        if(rc) {
            for (int j = 0; j < i; j++)
                pthread_join(thread[j], NULL);
            result = FAIL;
        }
    }
    if(result>0){
        for (size_t i = 0; i < procs_cnt; i++) {
            pthread_join(thread[i], NULL);
        }
    }
    if(free_threads(thread) < 0)
        return FAIL;
    return result;
}


//получение размера файла(POSIX)
int GetFileSize(char* f){
    struct stat file_stat;
    stat(f,&file_stat);
    return file_stat.st_size;
}

//получение размера строки
int GetStringSize(char* string){
    int cnt = 0;
    while(string[cnt+1] != '\0')
        cnt++;
    return cnt;
}




int count_entries(FILE* file,char* filename, char* string){
    if(!file || !string || !filename)
        return FAIL;
    int procs_cnt = get_nprocs();
    int file_size = GetFileSize(filename);
    int string_size = GetStringSize(string);
    if(string_size > file_size)
        return FAIL;
    thread_data* data = create_data(filename,procs_cnt,string,file_size,string_size);
    if(!data) {
        return FAIL;
    }
    if(create_threads(procs_cnt,data) <0) {
        free_threads_data(data,procs_cnt);
        return FAIL;
    }
    int result = 0;
    for(int i =0; i < procs_cnt;i++){
        result += data[i].cnt;
    }
    if(free_threads_data(data,procs_cnt) < 0)
        return FAIL;
    return result;
}


int free_threads(pthread_t* thread){
    if(!thread)
        return FAIL;
    free(thread);
    return SUCCESS;
}

int free_threads_data(thread_data* data,int size){
    if(!data)
        return FAIL;
    for(int i = 0; i < size;i++){
        if(data[i].file)
            fclose(data[i].file);
    }
    free(data);
    return SUCCESS;
}
