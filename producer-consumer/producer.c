// producer.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shared_memory.c"  

#define NUM_ITEMS 10

void producer() {
    for (int item = 0; item < NUM_ITEMS; item++) {
        P(1);  

        P(0);  
        shm_ptr->buffer[shm_ptr->in] = item;  
        printf("Producer produced item %d at position %d\n", item+1, shm_ptr->in);
        shm_ptr->in = (shm_ptr->in + 1) % BUFFER_SIZE;
        V(0);  

        V(2);  
        sleep(1);  
    }
}

int main() {
    create_shared_memory();
    init_semaphores();
    producer();  
    cleanup_shared_memory_and_semaphores();  
    return 0;
}
