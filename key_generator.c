#include<stdio.h>
#include "key_generator.h"

void generate_keys(int key_count){
    FILE *file = fopen("keys.bin", "wb");
    if(file == NULL){
        perror("Unable to open the file\n");
        return;
    }
    
    for(int i = 1; i<=key_count; i++){
        // fprintf(file, "%d ", i);
        Key k;
        k.val = i;
        fwrite(&k, sizeof(Key), 1, file);
    }
    fclose(file);
}