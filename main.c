#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
//CUSTOM LIBRARIES BELOW
#include "data.h"
#include "key_generator.h"
#include "shared_memory.h"

#define BUFFER_SIZE 1024
#define RANDOM_FILE_PATH "/home/manirup/assignment2/producer-consumer-problem/practice/demofileXXXXXX" 




int main(int argc, char* argv[]) {
    unsigned int key_count = 100000;
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

    //CREATE PRODUCER PROCESS (pending)
    //CREATE CONSUMER PROCESS (pending)
    pid_t p2 = fork();
    if(p2==0)
    {
        shm->buffer[0] = produce_data(1,2,3,4);
        shmdt(shm);
        return 0;
    }
    waitpid(p2, NULL, 0);
    print_data(&(shm->buffer[0]));
    return 0;
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