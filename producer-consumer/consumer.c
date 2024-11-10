// consumer.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shared_memory.c"  

#define NUM_ITEMS 10

void consumer() {
    for (int item = 0; item < NUM_ITEMS; item++) {
        P(2);  
        P(0);  

        item = shm_ptr->buffer[shm_ptr->out];  
        printf("Consumer consumed item %d from position %d\n", item+1, shm_ptr->out);
        shm_ptr->out = (shm_ptr->out + 1) % BUFFER_SIZE;
        V(0);  

        V(1);  
        sleep(1);  
    }
}

int main() {
    create_shared_memory();
    init_semaphores();
    consumer();  
    cleanup_shared_memory_and_semaphores();  
    return 0;
}
