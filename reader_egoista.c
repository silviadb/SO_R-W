#include "common.h"
 
int removeProduct(struct ringBuffer * buffer){
 
 printf("\n+--------------------------------------+\n");
 printf("+   CONSUMING AND INSERTING PRODUCT    +\n");
 printf("+--------------------------------------+\n");
 
 int currentOut = buffer->out;
 struct product * data = (struct product *) &buffer[1];
 
 data[currentOut].code = NULL;
 strcpy( data[currentOut].name, "" );
 buffer->out++;
 
 if(buffer->out == buffer->size)
  buffer->out = 0;
 
 printf("Product [Code: %d , Name: %s] REMOVED from Slot: %d\n",
  data[currentOut].code, data[currentOut].name,currentOut);
 
}
 
int emptyBuffer(struct ringBuffer * buffer, int numberProducts,
         int semid, float sleepingTime){
 
int i;
 
for (i = 0; i < numberProducts && buffer->isOpen ; i++){
 
printf("DECREASING Semaphore's Value - FULL.\n");
semop(semid, &decFull, 1);
printf("DECREASING Semaphore's Value - MUTEX.\n");
semop(semid, &decMutex, 1);
removeProduct(buffer);
printf("INCREASING Semaphore's Value - MUTEX.\n");
semop(semid, &incMutex, 1);
printf("INCREASING Semaphore's Value - EMPTY.\n");
semop(semid, &incEmpty, 1);
 
printf("\n> Buffer info - In Pointer: %d , Out Pointer: %d\n",
 buffer->in, buffer->out);
 
sleep(sleepingTime);
 
}
}
 
int main(int argc, char **argv)
{
if(argc < 4)
 printf("Usage: consumer <Semaphore Key> <Shared Memory Key> <Sleeping Time> <Number of Products>");
else{
 
 int semaphoreKey = atoi(argv[1]);
 int sharedMemoryKey = atoi(argv[2]);
 int sleepingTime = atoi(argv[3]);
 int numberProducts = atoi(argv[4]);
 
 int semaphoreArrayIdentifier;
 
 semaphoreArrayIdentifier = semget (semaphoreKey, NSEM, IPC_PRIVATE);
 
 int sharedMemoryIdentifier;
 char *attachedMemoryPointer;
 
 sharedMemoryIdentifier = sharedMemoryIdentifier = shmget(sharedMemoryKey,
         sizeof(struct ringBuffer),
         PERM);
 
 int i;
 struct ringBuffer * retrieveBuffer;
 
 if ( !(sharedMemoryIdentifier < 0 || semaphoreArrayIdentifier < 0) ) {
 
  retrieveBuffer = (struct ringBuffer *)shmat(sharedMemoryIdentifier, 0, 0);
 
  if (retrieveBuffer==( struct ringBuffer *)-1) {
      perror("shmat");
  } else {
       
   emptyBuffer( retrieveBuffer, numberProducts,
         semaphoreArrayIdentifier,sleepingTime);
       shmdt(retrieveBuffer);
  }
  } else {
  perror("shmget");
 }
 
 printf("\n> Consumer with PID: %d TERMINATED\n",getpid());
}
 
return EXIT_SUCCESS;
}
