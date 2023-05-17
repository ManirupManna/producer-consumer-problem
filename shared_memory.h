#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#include "data.h"
#define BUFFER_SIZE 1024
typedef struct {
    Data buffer[BUFFER_SIZE];
    unsigned int producer_index;
    unsigned int consumer_index;
    unsigned int size;
    unsigned int all_data_consumed; // 0-> MORE DATA TO BE CONSUMED, 1-> ALL DATA HAS BEEN CONSUMED
    unsigned int items_to_produce;
} SharedMemory;

SharedMemory* create_shared_memory();
/*SharedMemory* create_shared_memory()
{
    int shmid;
    key_t key;
    SharedMemory* shm;
    char RANDOM_FILE_PATH[] = "/home/manirup/assignment2/producer-consumer-problem/practice/demofileXXXXXX";
    int file_descriptor = mkstemp(RANDOM_FILE_PATH);
    close(file_descriptor);

    key = ftok(".", getpid());
    shmid = shmget(key, sizeof(SharedMemory), IPC_CREAT | 0666);
    if(shmid==-1){
        perror("shmget");
        return NULL;
    }
    shm = (SharedMemory*)shmat(shmid, NULL, 0);
    if(shm==(SharedMemory*)-1)
    {
        perror("shmat");
        return NULL;
    }
    return shm;
}
*/