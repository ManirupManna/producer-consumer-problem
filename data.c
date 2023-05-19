#include<stdio.h>
#include <time.h>
#include "data.h"

Data produce_data(int serial_no, int key, int producer_pid){
    Data d;
    d.key = key;
    d.serial_number = serial_no;
    d.producer_pid = producer_pid;
    d.time_produced = time(NULL);
    return d;
}
void consume_data(Data* d, FILE* file)
{
    d->time_consumed = time(NULL);
    fwrite(&(*d), sizeof(Data), 1, file);
    d->serial_number = 0;
}
void print_data(const Data* d){
    printf("Data details: serial_no: %i key: %i producer_pid: %i time_produced: %ld time_consumed: %ld\n", d->serial_number, d->key, d->producer_pid, (long)d->time_produced, (long)d->time_consumed);
}
