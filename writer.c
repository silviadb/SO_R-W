#include "common.h"
 
int insertProduct(struct ringBuffer * buffer, int code,
    char * name, int semid){
 
 printf("\n+--------------------------------------+\n");
 printf("+   PRODUCING AND INSERTING PRODUCT    +\n");
 printf("+--------------------------------------+\n");
 
 int currentIn = buffer->in;
  
 struct product * data = (struct product *)&buffer[1];
 data[currentIn].code = code;
 strcpy( data[currentIn].name, name );
 buffer->in++;
 
 if(buffer->in == buffer->size)
  buffer->in = 0;
 
 printf("Product [Code: %d , Name: %s] INSERTED in Slot: %d\n",
  data[currentIn].code,data[currentIn].name,currentIn);
 
}
 
int fillBuffer(struct ringBuffer * buffer, int numberProducts,
        int semid, float sleepingTime){
 
 int i;
 char name [10];
 
 for (i = 0; i < numberProducts  && buffer->isOpen; i++){
 
 strcpy(name, "");
 strcat(name,"PID:");
 //char code []= {(char) (65 + i), '\0'};
 char codex[10];
 snprintf(codex, 10,"%d",(int)getpid());
 strcat( name,  codex);
 
char text[100];
time_t now = time(NULL);
struct tm *t = localtime(&now);
strftime(text, sizeof(text)-1, "%d %m %Y %H:%M", t);
strcat(name,",Date:");
strcat( name,  text);

 
 printf("DECREASING Semaphore's Value - EMPTY.\n");
 semop(semid, &decEmpty, 1);
 printf("DECREASING Semaphore's Value - MUTEX.\n");
 semop(semid, &decMutex, 1);
 insertProduct(buffer,i,name,0);
 printf("INCREASING Semaphore's Value - MUTEX.\n");
 semop(semid, &incMutex, 1);
 printf("INCREASING Semaphore's Value - FULL.\n");
 semop(semid, &incFull, 1);
 
 printf("\n> Buffer info - In Pointer: %d , Out Pointer: %d\n"
  ,buffer->in, buffer->out);
 
 sleep(sleepingTime);
 
 }
}
 
int main(int argc, char **argv)
{
 
if(argc < 5)
 printf("Usage: producer <Semaphore Key> <Shared Memory Key> <Sleeping Time> <Number of Products>");
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
 

//______________________________________
int p_count;
pid_t pid;
pid_t proc[2];

   /* spawn writer processes */
  for (p_count = 1; p_count <= 2; p_count++)
     { if (-1 == (pid = fork())) /* spawn child process */
       { perror ("error in fork");  
          exit (1);
        }

//______________________________________

       if (0 == pid)             
        { /* processing for parent == writer */
          printf ("The writer process %d begins.\n", p_count);
          //printf("The child's PID is %d.  The process group ID is %dn",
         //  (int) getpid(), (int) getpgrp());

//_______________________________________


 if ( !(sharedMemoryIdentifier < 0 || semaphoreArrayIdentifier < 0) ) {
 
  retrieveBuffer = (struct ringBuffer *)shmat(sharedMemoryIdentifier, 0, 0);
 
  if (retrieveBuffer==( struct ringBuffer *)-1) {
      perror("shmat");
  } else {
       
   fillBuffer( retrieveBuffer, numberProducts,
        semaphoreArrayIdentifier,sleepingTime);
       shmdt(retrieveBuffer);
  }
  } else {
  perror("shmget");
 }
 
 printf("\n> Producer with PID: %d TERMINATED\n",getpid());
}
//______________
else proc[p_count -1]=pid;
}
}

//_____________________

return EXIT_SUCCESS;
}
