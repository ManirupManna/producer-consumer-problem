#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdatomic.h>
#include <time.h>
//CUSTOM LIBRARIES BELOW
#include "data.h"
#include "key_generator.h"
#include "shared_memory.h"
// #include "producer.h"

// #define BUFFER_SIZE 10
#define SLEEP_TIME 10 //in miliseconds // when producer can't produce or consumer can't consume
#define CONSUMER_SLEEP_TIME 100 // for changing consumer speed
#define PRODUCER_SLEEP_TIME 100 // for changing producer speed

// void producer(SharedMemory* shm);
void producer(SharedMemory* shm);
void consumer(SharedMemory* shm);
void verifier();

int main(int argc, char* argv[]) {
    unsigned int key_count = 10000;
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
    shm->itemCount = key_count;
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
    int corrupt;
    // printf("Now you can corrupt the keys file");
    // scanf("%d", &corrupt);
    verifier();
    printf("ALL OPERATIONS COMPLETED\n");
    return 0;
}
//OLD PRODUCER (using keys.txt)
/*
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
    munmap(file_data, file_size);
    close(file_descriptor);
}
*/
void producer(SharedMemory* shm){
    printf("PRODUCER STARTED\n");
    //READING KEYS FROM THE FILE
    int file_descriptor = open("keys.bin", O_RDONLY);
    if(file_descriptor==-1)
    {
        perror("Unable to open keys.bin");
        exit(1);
    }
    struct stat file_info;
    if(fstat(file_descriptor, &file_info)==-1){
        perror("Unable to get file information");
        close(file_descriptor);
        exit(1);
    }
    off_t file_size = file_info.st_size;
    Key *file_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
    if(file_data==(Key*)MAP_FAILED){
        perror("Unable to map the file into memory");
        close(file_descriptor);
        exit(1);
    }
    off_t i = 0; // FILE DESCRIPTOR INDEX
    int serial_no = 1; // PRODUCT SERIAL NO
    int produced_item_count = 1; // COUNT OF PRODUCED ITEMS
    int size = file_size / sizeof(Key);
    while (i<size)
    {
        unsigned int key = file_data[i].val;
        while(shm->buffer[shm->producer_index].serial_number!=0)// NO SPACE AVAILABLE IN BUFFER FOR PRODUCTION SO WAIT
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
        // usleep(PRODUCER_SLEEP_TIME*1000); //TO SLOW DOWN PRODUCER
    }
    munmap(file_data, file_size);
    close(file_descriptor);
}

void consumer(SharedMemory* shm){
    printf("CONSUMER STARTED\n");
    //OPENING THE LOG FILE for maintaing the record (.json file)
    FILE * file = fopen("LOG.bin", "wb");
    if(file==NULL){
        perror("Unable to open the LOG file.\n");
        exit(1);
    }
    int consumed_item_count = 0;
    while(consumed_item_count < shm->itemCount)
    {
        while(shm->buffer[shm->consumer_index].serial_number==0) //NOTHING TO CONSUME IN BUFFER SO WAIT
        {
            usleep(SLEEP_TIME*1000);
            continue;
        }
        consume_data(&shm->buffer[shm->consumer_index], file);
        int index = (shm->consumer_index + 1)%shm->size;
        shm->consumer_index = index;
        consumed_item_count++;
        // usleep(CONSUMER_SLEEP_TIME*1000); // TO SLOW DOWN THE CONSUMER
    }
    fclose(file);
}
//VERIFIER CHECKS IF PRODUCER CONSUMER OPERATIONS HAD BEEN VALID OR NOT
//right now verifier just reads the file that had been written by consumer

void verifier(){ 
    printf("VERIFIER STARTED\n");
    int log_file = open("LOG.bin", O_RDONLY);
    if(log_file==-1)
    {
        perror("Failed to open the file");
        exit(1);
    }
    struct stat log_file_info;
    if(fstat(log_file, &log_file_info)==-1){
        perror("Unable to get log file information");
        exit(1);
    }
    off_t log_file_size = log_file_info.st_size;
    Data* log_file_data = mmap(NULL, log_file_size, PROT_READ, MAP_PRIVATE, log_file, 0);
    if(log_file_data==(Data*)MAP_FAILED){
        perror("Unable to map the log file into memory");
        close(log_file);
        exit(1);
    }
    int key_file = open("keys.bin", O_RDONLY);
    if(key_file==-1)
    {
        perror("Failed to open the keys file");
        close(log_file);
        exit(1);
    }
    struct stat key_file_info;
    if(fstat(key_file, &key_file_info)==-1)
    {
        perror("Unable to get keys file information");
        close(log_file);
        close(key_file);
        exit(1);
    }
    off_t key_file_size = key_file_info.st_size;
    Key* key_file_data = mmap(NULL, key_file_size, PROT_READ, MAP_PRIVATE, key_file, 0);
    off_t index = 0;
    off_t log_count = log_file_size/sizeof(Data);
    off_t key_count = key_file_size/sizeof(Key);
    //CHECK IF KEY COUNT IS EQUAL TO THE NUMBER OF LOG RECORDS
    if(log_count!=key_count){
        if(log_count<key_count)
        {
            perror("You haven't processed all keys.");
        }
        else
        {
            perror("How can there by more logs than keys ?");
        }
        close(log_file);
        close(key_file);
        exit(1);
    }
    //CHECK IF SERIAL_NO IS INCREASING BY 1, KEY IS MATCHING WITH THE KEYS FILE, TIME_CONSUMED >= TIME_PRODUCED
    int prev_serial_no = 0; //the actual key value is starting from 1
    time_t last_production_time = 0;
    time_t last_consumption_time = 0;
    while(index<log_count){
        // log_file_data[index];
        // print_data(&log_file_data[index]);
        int key_in_keys_file = key_file_data[index].val;
        Data logs = log_file_data[index];
        //KEY in keys file and KEY in logs file are not matching
        if(key_in_keys_file!=logs.key){
            perror("keys are not matching\n");
            printf("key in file: %d, key in log: %d", key_in_keys_file, logs.key);
            close(key_file);
            close(log_file);
            exit(1);
        }
        //SERIAL NO is not followed
        else if(logs.serial_number!=prev_serial_no+1){
            perror("Sequence follwoing issue for Consumer(1)\n");
            printf("prev serial no: %d, current serial no: %d", prev_serial_no, logs.serial_number);
            close(key_file);
            close(log_file);
            exit(1);
        }

        else if((logs.time_produced<last_production_time) || (logs.time_consumed<last_consumption_time) || (logs.time_consumed<logs.time_produced)){
            perror("There is some issue with the production and consumption\n");
            printf("FOR THE LAST PRODUCT:\n");
            printf("production time: %ld, comsumption time: %ld\n", last_production_time, last_consumption_time);
            
            printf("FOR THE CURRENT PRODUCT:\n");
            printf("production time: %ld, comsumption time: %ld\n", logs.time_produced, logs.time_consumed);
            close(key_file);
            close(log_file);
            exit(1);
        }
        last_production_time = logs.time_produced;
        last_consumption_time = logs.time_consumed;
        prev_serial_no = logs.serial_number;
        index++;
    }
    printf("LOG DATA VERIFIED\n");
    close(log_file);
    close(key_file);
}