#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define BUFFER_SIZE 10        // Size of the buffer

// Semaphore operations
#define MUTEX 0   // Mutex for mutual exclusion
#define EMPTY 1   // Semaphore for empty slots
#define FULL 2    // Semaphore for full slots

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;  // Points to the next empty slot for the producer
    int out; // Points to the next filled slot for the consumer
} SharedBuffer;

// Function to perform semaphore wait operation
void sem_wait(int sem_id, int sem_num) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = -1; // Decrement (wait)
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}

// Function to perform semaphore signal operation
void sem_signal(int sem_id, int sem_num) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = 1; // Increment (signal)
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}

int main() {
    // Create shared memory segment with IPC_PRIVATE
    int shm_id = shmget(IPC_PRIVATE, sizeof(SharedBuffer), 0666 | IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    // Attach to the shared memory segment
    SharedBuffer* buffer = (SharedBuffer*) shmat(shm_id, NULL, 0);
    if ((intptr_t)buffer == -1) {
        perror("shmat failed");
        exit(1);
    }

    // Create a semaphore set with IPC_PRIVATE
    int sem_id = semget(IPC_PRIVATE, 3, 0666 | IPC_CREAT);
    if (sem_id == -1) {
        perror("semget failed");
        exit(1);
    }

    // Initialize semaphores: MUTEX = 1, EMPTY = BUFFER_SIZE, FULL = 0
    semctl(sem_id, MUTEX, SETVAL, 1);
    semctl(sem_id, EMPTY, SETVAL, BUFFER_SIZE);
    semctl(sem_id, FULL, SETVAL, 0);

    // Consume items
    for (int i = 0; i < 20; i++) {
        sem_wait(sem_id, FULL);      // Wait for a full slot
        sem_wait(sem_id, MUTEX);     // Enter critical section

        int item = buffer->buffer[buffer->out];  // Consume item
        printf("Consumer: Consumed item %d from index %d\n", item, buffer->out);
        buffer->out = (buffer->out + 1) % BUFFER_SIZE;

        sem_signal(sem_id, MUTEX);     // Exit critical section
        sem_signal(sem_id, EMPTY);     // Signal that the buffer has an empty slot

        sleep(1);  // Simulate time taken to consume an item
    }

    // Detach from shared memory
    shmdt(buffer);

    // Optionally remove shared memory and semaphores
    shmctl(shm_id, IPC_RMID, NULL);  // Remove shared memory
    semctl(sem_id, 0, IPC_RMID);     // Remove semaphores

    return 0;
}
