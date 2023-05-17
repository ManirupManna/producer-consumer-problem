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
    fprintf(file, "{\"serial_number\": \"%d\", \"key\": \"%d\", \"producer_pid\": \"%d\", \"time_produced\": \"%ld\", \"time_consumed\":\"%ld\"}, ", d->serial_number, d->key, d->producer_pid, (long)d->time_produced, (long)time(NULL));
    d->serial_number = 0;
}
void print_data(const Data* d){
    printf("Data details: serial_no: %i key: %i producer_pid: %i time_produced: %ld\n", d->serial_number, d->key, d->producer_pid, (long)d->time_produced);
}
