#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdatomic.h>
#include <time.h>
#include <unistd.h>
//CUSTOM LIBRARIES BELOW
#include "data.h"
#include "key_generator.h"
#include "shared_memory.h"

#define BUFFER_SIZE 1024
#define SLEEP_TIME 100 //in miliseconds

void producer(SharedMemory* shm);
void consumer(SharedMemory* shm);

int main(int argc, char* argv[]) {
    unsigned int key_count = 1000;
    unsigned int buffer_size = 1024;
    //ASSIGN VALUES TO KEY_COUNT, BUFFER_SIZE
    if(argc==2)
    {
        key_count = atoi(argv[1]);
    }
    if(argc>=3)
    {
        key_count = atoi(argv[1]);
        // buffer_size = atoi(argv[2]);
    }
    //GENERATE KEYS AND WRITE THEM IN A FILE CALLED KEYS.TXT
    pid_t p1 = fork();
    if(p1==0)
    {
        generate_keys(key_count);
        return 0;
    }
    waitpid(p1, NULL, 0);
    //CREATE SHARED MEMORY
    SharedMemory* shm = create_shared_memory();
    shm->size = BUFFER_SIZE;
    //CREATE PRODUCER PROCESS (pending)
    pid_t p2 = fork();
    if(p2==0){
        producer(shm);
        exit(0);
    }
    //CREATE CONSUMER PROCESS (pending)
    pid_t p3 = fork();
    if(p3==0){
        consumer(shm);
        exit(0);
    }
    waitpid(p2, NULL, 0);
    waitpid(p3, NULL, 0);
    /*
    pid_t p2 = fork();
    if(p2==0)
    {
        shm->buffer[0] = produce_data(1,2,3,4);
        shmdt(shm);
        return 0;
    }
    waitpid(p2, NULL, 0);
    print_data(&(shm->buffer[0]));
    printf("%p", &(shm->buffer[1]));
    print_data(&(shm->buffer[1]));
    */
    // producer(shm);
    printf("ALL OPERATIONS COMPLETED\n");
    return 0;
}

void producer(SharedMemory* shm){
    //READING KEYS FROM THE FILE
    int file_descriptor = open("keys.txt", O_RDONLY);
    if(file_descriptor==-1)
    {
        perror("Unable to open keys.txt");
        exit(1);
    }
    struct stat file_info;
    if(fstat(file_descriptor, &file_info)==-1){
        perror("Unable to get file information");
        close(file_descriptor);
        exit(1);
    }
    off_t file_size = file_info.st_size;
    char *file_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
    if(file_data==MAP_FAILED){
        perror("Unable to map the file into memory");
        close(file_descriptor);
        exit(1);
    }
    off_t i = 0; // FILE DESCRIPTOR INDEX
    int serial_no = 1; // PRODUCT SERIAL NO
    int produced_item_count = 1; // COUNT OF PRODUCED ITEMS
    while (i<file_size)
    {
        unsigned int key = 0;
        while(file_data[i]!=' '){
            // printf("%c->%i\n", file_data[i], file_data[i]-48);
            key = key *10 +  (file_data[i]-48);
            i++;
        }
        while(shm->buffer[shm->producer_index].serial_number!=0)// if producer index has data then wait
        {
            usleep(SLEEP_TIME*1000);
            continue;
        }
        shm->buffer[shm->producer_index] = produce_data(serial_no, key, getpid());
        int index = (shm->producer_index + 1) % shm->size;
        shm->producer_index = index;
        produced_item_count++;
        serial_no++;
        i++;
    }
    shm->all_data_consumed = 1;
    munmap(file_data, file_size);
    close(file_descriptor);
}

void consumer(SharedMemory* shm){
    //OPENING THE LOG FILE for maintaing the record (.json file)
    FILE* file1 = fopen("LOG.json", "w");
    if(file1==NULL){
        perror("Unable to open LOG file.\n");
        exit(1);
    }
    fprintf(file1, "[");
    fclose(file1);
    FILE * file = fopen("LOG.json", "a");
    if(file==NULL){
        perror("Unable to open the LOG file.\n");
        exit(1);
    }
    int consumed_item_count = 0;
    while(shm->all_data_consumed==0)
    {
        while(shm->buffer[shm->consumer_index].serial_number==0 && shm->all_data_consumed==0) //if the consumer index is empty wait
        {
            usleep(SLEEP_TIME*1000);
            continue;
        }
        consume_data(&shm->buffer[shm->consumer_index], file);
        int index = (shm->consumer_index + 1)%shm->size;
        shm->consumer_index = index;
        consumed_item_count++;
    }
    fprintf(file, "]");
    fclose(file);
}
/*
COMPILE
gcc main.c key_generator.c data.c -o program 
EXECUTE
./program
*/

/*
Data d = produce_data(0, 1, 101, 100000);
    print_data(&d);
    printf("%i %i %i %lld\n", d.serial_number, d.key, d.producer_pid, d.time_produced);
    return 0;
*/