#include "common.h"
 
int removeProduct(struct ringBuffer * buffer){
 
	 printf("\n+--------------------------------------+\n");
	 printf("+   CONSUMING AND INSERTING PRODUCT    +\n");
	 printf("+--------------------------------------+\n");
	 
	 int currentOut = buffer->out;
	 struct product * data = (struct product *) &buffer[1];
	 
	 data[currentOut].code = NULL;
	// strcpy( data[currentOut].name, "" );
	 buffer->read++;
	 
	 if(buffer->read == buffer->size)
	  buffer->read = 0;
	 
	 printf("Product [Code: %d , Name: %s] read from Slot: %d\n",
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
 
void * crear_reader(struct hilo_rw *arg){

 int semaphoreArrayIdentifier = arg->s_key;
 int sharedMemoryIdentifier = arg->m_key;
 int sleepingTime = arg->sleep;
 int numberProducts = arg->num_p;

 pthread_t h_aux;
 pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;


 int i;
 struct ringBuffer * retrieveBuffer;
 
 if ( !(sharedMemoryIdentifier < 0 || semaphoreArrayIdentifier < 0) ) {
 
  retrieveBuffer = (struct ringBuffer *)shmat(sharedMemoryIdentifier, 0, 0);
 
  if (retrieveBuffer==( struct ringBuffer *)-1) {
      perror("shmat");
  } else {
  
    int f=0;
   
    pthread_mutex_lock (&mutex1);
   emptyBuffer( retrieveBuffer, numberProducts,
         semaphoreArrayIdentifier,sleepingTime);
       shmdt(retrieveBuffer);
   pthread_mutex_unlock (&mutex1);
  
  }
  } else {
  perror("shmget");
 }
 
  crear_reader(arg);
  h_aux = pthread_self();
 
 printf("\n> Reader con PID: %d TERMINATED\n",(unsigned int) h_aux);

 
return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
 
if(argc < 5)
 printf("Use: reader <Semaphore Key> <Shared Memory Key> <Sleeping Time> <Number of Products>");
else{

 int semaphoreKey = atoi(argv[1]);
 int sharedMemoryKey = atoi(argv[2]);
 int sleepingTime = atoi(argv[3]);
 int numberProducts = atoi(argv[4]);
 int i;
 pthread_t hilos[numberProducts];
 

  
 int semaphoreArrayIdentifier;
 
 semaphoreArrayIdentifier = semget (semaphoreKey, NSEM, IPC_PRIVATE);
 
 int sharedMemoryIdentifier;
 char *attachedMemoryPointer;
 
 sharedMemoryIdentifier = sharedMemoryIdentifier = shmget(sharedMemoryKey,
         sizeof(struct ringBuffer),
         PERM);

 struct hilo_rw h_r;
   h_r.s_key=semaphoreArrayIdentifier;
   h_r.m_key=sharedMemoryIdentifier;
   h_r.sleep = sleepingTime;
   h_r.num_p = numberProducts;

 for(i=0;i<numberProducts;i++){
 	int new_reader = pthread_create(&hilos[i], NULL, (void *) &crear_reader,(void *) &h_r);
 }

  sleep(50);
  
  for(i=1;i<numberProducts;i++){
     pthread_join(hilos[i], NULL);
  }
 
}
 return 0;
}
