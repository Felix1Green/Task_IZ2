#include <lib_hdr.h>
#include "func.h"


int main(int argc, char* argv[]) {
    FILE *test = NULL;
    test = fopen(PATH_TO_FILE, "r");
    if (!test) {
        printf("FILE_ERROR\n");
        return FAILED;
    }
    printf("Enter the required string\n");
    char *string = input_str(stdin);
    if (!string) {
        fclose(test);
        return (puts("REQUIRED STRING ERROR"));
    }
    long start = time_sec();
    int result = count_entries(test, string);
    long end = time_sec();
    printf("result: %d\n", result);
    printf("static library spends %ld ms", end - start);
    free(string);
    return result;
}
