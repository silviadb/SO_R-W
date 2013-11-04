#include "common.h"
 
int main(int argc, char **argv)
{
if(argc < 3)
 printf("Usage: producer <Semaphore Key> <Shared Memory Key>");
else{
 
int semaphoreKey = atoi(argv[1]);
int sharedMemoryKey = atoi(argv[2]);
 
int sharedMemoryIdentifier;
char *attachedMemoryPointer;
 
int semaphoreIdentifier;
 
sharedMemoryIdentifier = shmget(sharedMemoryKey, sizeof(struct ringBuffer), PERM);
 
int i;
struct ringBuffer * retrieveBuffer;
 
while(1){
 
 if (sharedMemoryIdentifier >= 0) {
 
  retrieveBuffer = (struct ringBuffer *)shmat(sharedMemoryIdentifier, 0, 0);
 
  if (retrieveBuffer==(struct ringBuffer *)-1) {
      perror("shmat");
  }
  else {
  printf("+----------------------------------------------+\n");
  printf("+   BUFFER INFORMATION    Is Open       [%d]   +\n",
   retrieveBuffer->isOpen);
  printf("+----------------------------------------------+\n");
  printf("| I | O |   Slot #   |    Code    |    Name    |\n");
  printf("+----------------------------------------------+\n");
  for(i=0;i<retrieveBuffer->size;i++){
   struct product *data = (struct product *)&retrieveBuffer[1];
   struct product currentProduct = data[i];
   printf("| %c | %c | %10d | %10d | %10s |\n",
    retrieveBuffer-> in == i ? '>' : ' '
    , retrieveBuffer-> out == i ? '>' : ' '
    , i,currentProduct.code,currentProduct.name);
   printf("+---------------------------------------------+\n");
  }
 
  shmdt(retrieveBuffer);
  }
 }
 else {
  perror("shmget");
 }
 
 sleep(1);
}
}
return EXIT_SUCCESS;
}
