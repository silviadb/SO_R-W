#include "common.h"
 
 
int main(int argc, char **argv)
{
 
if(argc < 3)
 printf("Usage: producer <semaphore key=""> <shared key="" memory="">");
else{
 
 int semaphoreKey = atoi(argv[1]);
 int sharedMemoryKey = atoi(argv[2]);
 
 int sharedMemoryIdentifier;
 
 printf("\n+--------------------------------------+\n");
 printf("+     DEALLOCATIONG SHARED MEMORY      +\n");
 printf("+--------------------------------------+\n");
 
 
 sharedMemoryIdentifier = shmget(sharedMemoryKey,
     sizeof(struct ringBuffer), PERM);
 
 if (sharedMemoryIdentifier >= 0) {
  if(shmctl(sharedMemoryIdentifier, IPC_RMID, 0) > -1)
  {
   printf ("Shared-Memory with ID: %d unallocated.\n",
    sharedMemoryIdentifier);
  }
   
 }
 else {
  perror("Invalid Shared-Memory Key.");
 }
 printf("\n+--------------------------------------+\n");
 printf("+      REMOVING SEMAPHORE ARRAY        +\n");
 printf("+--------------------------------------+\n");
 
 int semaphoreIdentifier;
 
 semaphoreIdentifier = semget (semaphoreKey, 0, IPC_PRIVATE);
 
 if (semaphoreIdentifier >= 0) {
  if (semctl (semaphoreIdentifier, 0, IPC_RMID, 0) > -1)
  {
   printf ("Semaphores-Array with ID: %d unallocated.\n",
    semaphoreIdentifier);
  }
 }
 else {
  perror("Invalid Semaphores-Array Key.");
 }
}
}
