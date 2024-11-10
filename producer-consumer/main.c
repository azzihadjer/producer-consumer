
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shared_memory.c"  

void create_producer_and_consumer_processes() {
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        producer(); 
        exit(0);
    }

    pid = fork();
    if (pid == 0) {
        consumer();  
        exit(0);
    }
}

int main() {
    create_shared_memory();
    init_semaphores();
    create_producer_and_consumer_processes();  

    wait(NULL);  
    wait(NULL);

    cleanup_shared_memory_and_semaphores();  
    return 0;
}
