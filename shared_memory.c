#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#include "shared_memory.h"
SharedMemory* create_shared_memory(){
    int shmid;
    key_t key;
    SharedMemory* shm;
    char RANDOM_FILE_PATH[] = "/home/manirup/assignment2/producer-consumer-problem/practice/randomFiles/demofileXXXXXX";
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
    shm->producer_index = 0;
    shm->consumer_index = 0;
    shm->all_data_consumed = 0;
    return shm;
}