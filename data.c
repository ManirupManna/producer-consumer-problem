#include<stdio.h>
#include "data.h"

Data produce_data(int serial_no, int key, int producer_pid, int time_produced){
    Data d;
    d.key = key;
    d.serial_number = serial_no;
    d.producer_pid = producer_pid;
    d.time_produced = time_produced;
    return d;
}

void print_data(const Data* d){
    printf("Data details: serial_no: %i key: %i producer_pid: %i time_produced: %lld\n", d->serial_number, d->key, d->producer_pid, d->time_produced);
}
