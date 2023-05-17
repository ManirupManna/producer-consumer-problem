#include<stdio.h>
#include "key_generator.h"

void generate_keys(int key_count){
    FILE *file = fopen("keys.txt", "w");
    if(file == NULL){
        printf("Unable to open the file\n");
        return;
    }
    
    for(int i = 1; i<=key_count; i++){
        fprintf(file, "%010d", i);
    }
    fclose(file);
}