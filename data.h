#ifndef DATA_H
#define DATA_H


typedef struct
{
    int key;
    int producer_pid;
    int serial_number;
    long long time_produced;
    long long time_consumed;
    
} Data;

Data produce_data(int serial_no, int key, int producer_pid, int time_produced);

void print_data(const Data* d);

#endif