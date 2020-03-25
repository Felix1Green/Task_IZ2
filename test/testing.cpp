#include "gtest/gtest.h"
#include <iostream>
#define ERROR -1
extern "C" {
#include "func.h"
#include "lib_def.h"
#include "lib_hdr.h"
#include <dlfcn.h>
#include <pthread.h>
}
FILE* t;
FILE*st;

TEST(static_equal_result,ok){
    if((t = fopen(PATH_TO_FILE,"r")) == NULL){
        puts("FILE ERROR 1\n");
        assert(false);
    }
    if((st = fopen(PATH_TO_STFILE_TEST,"r")) == NULL){
        puts("FILE_ERROR\n");
        assert(false);
    }
    char* string = input_str(st);
    int result = count_entries(t,string);
    free(string);
    fclose(t);
    fclose(st);
    EXPECT_EQ(result,7);
}

TEST(static_fail_result,not_ok){
    char* string = NULL;
    int result = count_entries(t,string);
    EXPECT_EQ(result,ERROR);
}

//------------------dynamic_lib_TEST------------------

class Function{
public:
    void* library;
    int (*GetFileSize)(char* f);
    int (*GetStringSize)(char* string);
    thread_data* (*create_data)(char* f,int procs_cnt, char* string,int file_size,int string_size);
    int (*create_threads)(int procs_cnt,thread_data* data);
    int (*free_threads_data)(thread_data* data, int size);
    int (*free_threads)(pthread_t* thread);
    int (*count_entries)(FILE* file,char* filename, char* string);
};


class Function_Sym : public ::testing::Test{
protected:
    void SetUp() override{
        func.library = dlopen(PATH_TO_DYN_LIB, RTLD_LAZY);
        *(void**)(&func.free_threads) = dlsym(func.library,"free_threads");
        *(void**)(&func.count_entries) = dlsym(func.library,"count_entries");
        *(void**)(&func.GetStringSize) = dlsym(func.library,"GetStringSize");
        *(void**) (&func.GetFileSize) = dlsym(func.library,"GetFileSize");
        *(void**)(&func.create_data) = dlsym(func.library,"create_data");
        *(void**)(&func.create_threads) = dlsym(func.library,"create_threads");
        *(void**)(&func.free_threads_data) = dlsym(func.library,"free_threads_data");
    }
    void TearDown() override {
        dlclose(func.library);
    }
    Function func{};
};

TEST_F(Function_Sym, ct_entries){
    if((t = fopen(PATH_TO_FILE,"r")) == NULL){
        puts("FILE ERROR 1\n");
        assert(false);
    }
    if((st = fopen(PATH_TO_STFILE_TEST,"r")) == NULL){
        puts("FILE_ERROR\n");
        assert(false);
    }
    char* str = input_str(st);
    int ans = func.count_entries(t,(char*)PATH_TO_FILE,str);
    free(str);
    fclose(t);
    fclose(st);
    EXPECT_EQ(ans,7);
}

TEST_F(Function_Sym,get_file_size){
    if((t = fopen(PATH_TO_FILE,"r")) == NULL){
        puts("FILE ERROR 1\n");
        assert(false);
    }
    int res = func.GetFileSize((char*)PATH_TO_FILE);
    fclose(t);
    EXPECT_EQ(res,21);
}

TEST_F(Function_Sym,get_string_size){
    if((st = fopen(PATH_TO_STFILE_TEST,"r")) == NULL){
        puts("FILE ERROR 1\n");
        assert(false);
    }
    char* str = input_str(st);
    int res = func.GetStringSize((char*)str);
    free(str);
    fclose(st);
    EXPECT_EQ(res,0);
}

TEST_F(Function_Sym,cnt_entries_not_ok){
    char* string = NULL;
    int res = func.count_entries(t,(char*)PATH_TO_FILE,string);
    EXPECT_EQ(res,-1);
}

TEST_F(Function_Sym,cnt_entries_not_ok_2){
    if((t = fopen(PATH_TO_FILE,"r")) == NULL){
        puts("FILE ERROR 1\n");
        assert(false);
    }
    if((st = fopen(PATH_TO_STBIGGER_TEST,"r")) == NULL){
        puts("FILE_ERROR\n");
        assert(false);
    }
    char* str = input_str(st);
    int ans = func.count_entries(t,(char*)PATH_TO_FILE,str);
    free(str);
    fclose(t);
    fclose(st);
    EXPECT_EQ(ans,-1);
}

TEST_F(Function_Sym,free_threads_data_not_ok){
    int res = func.free_threads_data(NULL,0);
    EXPECT_EQ(res,-1);
}
TEST_F(Function_Sym,free_threads_not_ok){
    int res = func.free_threads(NULL);
    EXPECT_EQ(res,-1);
}

TEST_F(Function_Sym,create_data_not_ok){
    thread_data* res = func.create_data(NULL,0,NULL,0,0);
    EXPECT_TRUE(res==NULL);
}

TEST_F(Function_Sym,create_data_ok){
    thread_data* res = func.create_data(PATH_TO_FILE,1,"sd",21,3);
    func.free_threads_data(res,1);
    EXPECT_FALSE(res == NULL);
}

TEST_F(Function_Sym,create_threads_ok){
    thread_data* data = func.create_data(PATH_TO_FILE,1,"str",21,3);
    int res = func.create_threads(1,data);
    func.free_threads_data(data,1);
    EXPECT_EQ(res,1);
}

//----------COMPARE_RESULTS---------------

TEST_F(Function_Sym,Compare_Res){
    if((t = fopen(PATH_TO_BIGFILE_TEST,"r")) == NULL){
        puts("FILE ERROR 1\n");
        assert(false);
    }
    if((st = fopen(PATH_TO_STFILE_TEST,"r")) == NULL){
        puts("FILE_ERROR\n");
        assert(false);
    }
    char* str = input_str(st);
    long start_dyn = time_sec();
    int res_dyn = func.count_entries(t,PATH_TO_BIGFILE_TEST,str);
    long end_dyn = time_sec();
    long start_st = time_sec();
    int res_st = count_entries(t,str);
    long end_st = time_sec();
    printf("\nDYNAMIC LIBRARY RESULT: %d REQUIRED TIME: %ld ms\n",res_dyn,end_dyn - start_dyn);
    printf("\nSTATIC LIBRARY RESULT: %d REQUIRED TIME: %ld ms\n",res_st,end_st - start_st);
    fclose(t);
    fclose(st);
    free(str);
    // как результат, динамическая прога работает намного(зависит от машины) быстрее статической
    EXPECT_EQ(res_dyn,res_st);
}
