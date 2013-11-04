#include "common.h"

int createSemaphores (int numberElements, int semaphoreKey) {
 
printf("\n+--------------------------------------+\n");
printf("+         SEMAPHORE CREATION           +\n");
printf("+--------------------------------------+\n");
 
int semaphoreIdentifier;
semaphoreIdentifier = semget (semaphoreKey, 0, IPC_PRIVATE);
 
if (semaphoreIdentifier < 0) {
 
 umask(0);
 semaphoreIdentifier = semget (semaphoreKey, NSEM,
          IPC_CREAT | IPC_EXCL | PERM);
 
 if (semaphoreIdentifier < 0) {
 perror("Failed to create semaphore array");
 exit (EXIT_FAILURE);
 }
 else{
  printf("> Semaphore Array successfully creted.\n");
 }
   
 if (semctl(semaphoreIdentifier, MUTEXSEM, SETVAL, 1) > 0) {
 perror("Failed to initialize MUTEXSEM");
 }
 else{
  printf("> MUTEXSEM initialized.\n");
 }
  
 if (semctl( semaphoreIdentifier, EMPTYSEM,
      SETVAL, numberElements)< 0) {
 perror("Failed to initialize EMPTYSEM");
 exit (EXIT_FAILURE);
 }
 else{
  printf("> EMPTYSEM initialized.\n");
 }
  
 if (semctl( semaphoreIdentifier, FULLSEM,
      SETVAL, 0)< 0) {
 perror("Failed to initialize FULLSEM.");
 exit (EXIT_FAILURE);
 }
 else{
  printf("> FULLSEM initialized.\n");
 }
 
 printf ("> Semaphore Array {MUTEX, EMPTY, FULL} created with ID: %d.\n"
  , semaphoreIdentifier);
}
 
return semaphoreIdentifier;
 
}
 
int createSharedMemory (int sharedMemoryKey){
 
printf("\n+--------------------------------------+\n");
printf("+      SHARED MEMORY ALLOCATION        +\n");
printf("+--------------------------------------+\n");
 
int sharedMemoryIdentifier = shmget(sharedMemoryKey,
        sizeof(struct ringBuffer),
        IPC_CREAT | PERM);
 
if (sharedMemoryIdentifier<0) {
 perror("shmget");
}
else{
 printf ("Shared Memory Slot allocated with ID: %d and Size: %d\n",
  sharedMemoryIdentifier, (int)sizeof(struct ringBuffer));
}
 
return sharedMemoryIdentifier;
}
 
int permissionToProceed(int semaphoresKey, int sharedMemoryKey)
{
int sharedMemoryIdentifier, semaphoreIdentifier, permissionToProceed;
 
permissionToProceed = 1;
 
sharedMemoryIdentifier = shmget(sharedMemoryKey, 0, 0);
semaphoreIdentifier = semget(semaphoresKey, 0, 0);
 
if(sharedMemoryIdentifier > 0)
{
 permissionToProceed--;
 printf("Shared-Memory with the given key already exists, unable to obtain shared memory.");
}
 
if(semaphoreIdentifier > 0)
{
 permissionToProceed--;
 printf("A Semaphore-Array with the given key already exists, unable to obtain Semaphore-Array.");
}
 
return permissionToProceed;
}
 
int main(int argc, char **argv)
{
 
if(argc < 4)
 printf("Usage: resource_create <Semaphore Key> <Shared Memory Key> <Number of Products>");
else
{
 
int semaphoreKey = atoi(argv[1]);
int sharedMemoryKey = atoi(argv[2]);
int numberProducts = atoi(argv[3]);
 
int sharedMemoryIdentifier;
char *attachedMemoryPointer;
int semaphoreArrayIdentifier;
 
if(permissionToProceed(semaphoreKey, sharedMemoryKey))
{
 
semaphoreArrayIdentifier = createSemaphores(numberProducts,semaphoreKey);
sharedMemoryIdentifier = createSharedMemory(sharedMemoryKey);
 
int i;
struct ringBuffer * retrieveBuffer;
 
if ( !(sharedMemoryIdentifier < 0 || semaphoreArrayIdentifier < 0) )
{
 
 retrieveBuffer = (struct ringBuffer *)shmat(sharedMemoryIdentifier, 0, 0);
 
 if (retrieveBuffer==( struct ringBuffer *)-1)
 {
     perror("Shmat");
 }
 else
 {
  retrieveBuffer->size = numberProducts;
  retrieveBuffer->isOpen = 1;
 
  printf("\n+--------------------------------------+\n");
  printf("+           BUFFER INFORMATION         +\n");
  printf("+--------------------------------------+\n");
 
  printf("In Pointer: %d , Out Pointer: %d\n",
   retrieveBuffer->in, retrieveBuffer->out);
 
  printf("Slots: %d , Is Open: %d\n",
   retrieveBuffer->size, retrieveBuffer->isOpen);
 
 }
}
else
{
 perror("Shmget");
}
}
else
{
 printf("The resources could not be created. Try again with new keys. \n");
}
}
return EXIT_SUCCESS;
}
