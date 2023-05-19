
#ifndef DATA_H
#define DATA_H
#include <time.h>

typedef struct
{
    int key;
    int producer_pid;
    int serial_number;
    time_t time_produced;
    time_t time_consumed;
} Data;

Data produce_data(int serial_no, int key, int producer_pid);

void consume_data(Data* d, FILE* file);

void print_data(const Data* d);

#endif