// shared_memory.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 5  // Size of the buffer

// Shared buffer structure
typedef struct {
    int buffer[BUFFER_SIZE];
    int in, out;
} SharedBuffer;

int semid;  // Semaphore ID
int shm_id;  // Shared memory ID
SharedBuffer *shm_ptr;  // Pointer to shared memory
struct sembuf sem_op;  // Semaphore operation structure

// Function to initialize semaphores
void init_semaphores() {
    semid = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 1);  // Mutex semaphore (binary semaphore)
    semctl(semid, 1, SETVAL, BUFFER_SIZE);  // Empty slots semaphore
    semctl(semid, 2, SETVAL, 0);  // Full slots semaphore
}

// Function to perform "wait" (P) operation on a semaphore
void P(int sem_num) {
    sem_op.sem_num = sem_num;
    sem_op.sem_op = -1;  // Decrement the semaphore
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
}

// Function to perform "signal" (V) operation on a semaphore
void V(int sem_num) {
    sem_op.sem_num = sem_num;
    sem_op.sem_op = 1;  // Increment the semaphore
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
}

// Function to create and attach shared memory
int create_shared_memory() {
    shm_id = shmget(IPC_PRIVATE, sizeof(SharedBuffer), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    shm_ptr = (SharedBuffer *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (SharedBuffer *)-1) {
        perror("shmat failed");
        exit(1);
    }

    shm_ptr->in = 0;
    shm_ptr->out = 0;
    return shm_id;
}

// Function to cleanup shared memory and semaphores
void cleanup_shared_memory_and_semaphores() {
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);  // Remove the semaphore set
}

