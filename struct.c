#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct abcd{
    int size;
    int* arr;
} S1;

int main(){
    /*Person p1;
    strcpy(p1.name, "Manirup");
    p1.age = 21;
    printf("%i %s\n", p1.age, p1.name);
    struct demo d;
    d.nothing = 12345;
    printf("%i\n", d.nothing);
    */
    //dynamic size allocation of struct array
    int arraySize;
    printf("Enter the size of array: ");
    scanf("%i", &arraySize);
    int memory_size = sizeof(S1) + arraySize * sizeof(int);
    // S1* p = malloc(memory_size);
    // p->size = arraySize;
    //creating shared memory
    key_t key;
    int shmid;
    S1* shm;
    int SHM_SIZE = memory_size;
    
        //generate key
        key = ftok(".", getpid());
        //create shared memory segment
        shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
        if(shmid == -1){
            perror("shmget");
            return 1;
        }
        //attach the shared memory segment to the process's address
        shm = (S1*)shmat(shmid, NULL, 0);
        if(shm==(S1*)-1){
            perror("shmat");
            return 1;
        }
        shm->size = arraySize;
        pid_t pid1 = fork();
        if(pid1==0){
            printf("Child Process 1 - PID: %d\n", getpid());
            // printf("Size of shared memory array: %d\n", shm->size);
            int *first = (void *)shm + sizeof(int); 
            *first = 100;
            printf("Value is %d\n", *first);
            printf("Child Pro 1 - PID: %d\n", getpid());
            //detach shared memory
            // shmdt(shm);
            exit(0);
        }
        // pid_t pid2 = fork();
        // if(pid2==0){
        //     printf("Child Process 2 - PID: %d\n", getpid());
        //     shm->arr[shm->size-1] = 100;
        //     printf("Child Proc 2 - PID: %d\n", getpid());
        //     //detach shared memory
        //     // shmdt(shm);
        //     exit(0);
        // }
    waitpid(pid1, NULL, 0);
    // waitpid(pid2, NULL, 0);
    // print from shared memory

    // for(int i = 0; i<arraySize; i++){
        int *first = (void *)shm + sizeof(int); 
        printf("arr[%d]=%d\n", 0, *first);
    // }
    return 0;
}