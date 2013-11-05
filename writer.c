#include "common.h"
 
int insertProduct(struct ringBuffer * buffer, int code,
    char * name, int semid){
 
 printf("\n+--------------------------------------+\n");
 printf("+   PRODUCING AND INSERTING Data    +\n");
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
 
void * crear_writer(struct hilo_rw *arg){
 
	 int semaphoreArrayIdentifier = arg->s_key;
	 int sharedMemoryIdentifier = arg->m_key;
	 int sleepingTime = arg->s_key;
	 int numberProducts = arg->s_key;

	 pthread_t h_aux;
	 	 
	 
	 int i;
	 struct ringBuffer * retrieveBuffer;


	 if ( !(sharedMemoryIdentifier < 0 || semaphoreArrayIdentifier < 0) ) {
	 
	  retrieveBuffer = (struct ringBuffer *)shmat(sharedMemoryIdentifier, 0, 0);
	 
	  if (retrieveBuffer==( struct ringBuffer *)-1) {
	      perror("shmat");
	  } else {
          int f=0;
	   while(f!=1){
	   fillBuffer( retrieveBuffer, numberProducts,
		semaphoreArrayIdentifier,sleepingTime);
	       shmdt(retrieveBuffer);
            }
	  }
	  } else {
	  perror("shmget");
	 }
	 
	 h_aux = pthread_self();
	 
	 printf("\n> Producer with PID: %d TERMINATED\n",(unsigned int) h_aux);


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
	 int i;
	 pthread_t hilos[numberProducts];

         int semaphoreArrayIdentifier;
	 
	 semaphoreArrayIdentifier = semget (semaphoreKey, NSEM, IPC_PRIVATE);
	 
	 int sharedMemoryIdentifier;
	 char *attachedMemoryPointer;
	 
	 sharedMemoryIdentifier = sharedMemoryIdentifier = shmget(sharedMemoryKey,
		 sizeof(struct ringBuffer),
		 PERM);
          
	 
	 struct hilo_rw h_w;
	   h_w.s_key=semaphoreArrayIdentifier;
	   h_w.m_key=sharedMemoryIdentifier;
	   h_w.sleep = sleepingTime;
	   h_w.num_p = numberProducts;

	 for(i=0;i<numberProducts;i++){
	 	int new_writer = pthread_create(&hilos[i], NULL, (void *) &crear_writer,(void *) &h_w);
	 }

	  sleep(50);
	  
	  for(i=1;i<numberProducts;i++){
	     pthread_join(hilos[i], NULL);
	  }
	 
	}
	 return 0;
}


