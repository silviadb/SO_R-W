#include "common.h"

// Funcion: crear semaforo
//Parametros: numero elementos , key del semaforo
int crear_sem (int num_elem, int sem_key) {
 
	printf("\n+--------------------------------------+\n");
	printf("+         Creacion de Semaforo           +\n");
	printf("+--------------------------------------+\n");
	 
	int sem_ident;
	sem_ident = semget (sem_key, 0, IPC_PRIVATE);
	 
	if (sem_ident < 0) {
	 
		 umask(0);
		 sem_ident = semget (sem_key, NSEM,
			  IPC_CREAT | IPC_EXCL | PERM);
		 
		 if (sem_ident < 0) {
			 perror("Fallo crear array de semaforos");
			 exit (EXIT_FAILURE);
		 }
		 else{
		  	printf("> Arreglo de semaforo creado correctamente.\n");
		 }
		   
		 if (semctl(sem_ident, MUTEXSEM, SETVAL, 1) > 0) {
		 	perror("Fallo inicializar: MUTEXSEM");
		 }
		 else{
		  	printf("> MUTEXSEM inicializado.\n");
		 }
		  
		 if (semctl( sem_ident, EMPTYSEM,SETVAL, num_elem)< 0) {
		 	perror("Fallo inicializar: EMPTYSEM");
		 	exit (EXIT_FAILURE);
		 }
		 else{
		  	printf("> EMPTYSEM inicializado.\n");
		 }
		  
		 if (semctl( sem_ident, FULLSEM,SETVAL, 0)< 0) {
		 	perror("Fallo inicializar: FULLSEM.");
		 exit (EXIT_FAILURE);
		 }
		 else{
		  printf("> FULLSEM inicializado.\n");
		 }
		 
		 printf ("> Semaforo array {MUTEX, EMPTY, FULL} creado con ID: %d.\n"
		  , sem_ident);
	}
	 
	return sem_ident;
 
}
//-----------------------------------------------------------------------------------------

// Funcion: crear_memoriacompartida
//Parametros:key del segmento de memoria
int crear_SharedMemory (int sharedMemory_key){
 
	printf("\n+--------------------------------------+\n");
	printf("+    Asignacion de Memoria Compartida    +\n");
	printf("+--------------------------------------+\n");
	 
	int sharedMemory_ident = shmget(sharedMemory_key,
                sizeof(struct ringBuffer),
		IPC_CREAT | PERM);
	 
	if (sharedMemory_ident<0) {
	 	perror("shmget");
	}
	else{
		 printf ("Memoria Compatida, ID: %d ,Tama~o: %d\n",
		  sharedMemory_ident, (int)sizeof(struct ringBuffer));
	}
	return sharedMemory_ident;
}

//-----------------------------------------------------------------------------------------

// Funcion: permiso de procesar
//Parametros:key del semaforo, key del segmento de memoria compartida
int permiso_procesar(int semaphoresKey, int sharedMemory_key)
{
	int sharedMemory_ident, sem_ident, permiso_procesar;
	 
	permiso_procesar = 1;
	 
	sharedMemory_ident = shmget(sharedMemory_key, 0, 0);
	sem_ident = semget(semaphoresKey, 0, 0);
	 
	if(sharedMemory_ident > 0)
	{
	 permiso_procesar--;
	 printf("Existe el key de la memoria compartida, habilitado para obtener memoria compartida.");
	}
	 
	if(sem_ident > 0)
	{
	 permiso_procesar--;
	 printf("Existe el key del semafor, habilitado para obtener semaforo.");
	}
	 
	return permiso_procesar;
}
 
int main(int argc, char **argv)
{
 
	if(argc < 4)
	 printf("Use: resource_create <Semaphore Key> <Shared Memory Key> <Number of Products>");
	else
	{
	 
		int sem_key = atoi(argv[1]);
		int sharedMemory_key = atoi(argv[2]);
		int num_prod = atoi(argv[3]);
		 
		int sharedMemory_ident;
		char *attachedMemoryPointer;
		int sem_array_ident;
		 
		if(permiso_procesar(sem_key, sharedMemory_key))
		{
		 
			sem_array_ident = crear_sem(num_prod,sem_key);
			sharedMemory_ident = crear_SharedMemory(sharedMemory_key);
			 
			int i;
			struct ringBuffer * Aux_Buffer;
			 
			if ( !(sharedMemory_ident < 0 || sem_array_ident < 0) )
			{
			 
				 Aux_Buffer = (struct ringBuffer *)shmat(sharedMemory_ident, 0, 0);
				 
				 if (Aux_Buffer==( struct ringBuffer *)-1)
				 {
				     perror("Shmat");
				 }
				 else
				 {
				  Aux_Buffer->size = num_prod;
				  Aux_Buffer->isOpen = 1;
				 
				  printf("\n+--------------------------------------+\n");
				  printf("+         Informacion del Buffer         +\n");
				  printf("+--------------------------------------+\n");
				 
				  printf("In Puntero: %d , Out Puntero: %d\n",
				   Aux_Buffer->in, Aux_Buffer->out);
				 
				  printf("Lineas: %d , Is Open: %d\n",
				   Aux_Buffer->size, Aux_Buffer->isOpen);
				 
				 }
			}
			else
			{
			 	perror("Shmget");
			}
		}
		else
		{
		 	printf("El recurso no pudo ser creado , intente de nuevo con una nueva key. \n");
		}
	}
	return EXIT_SUCCESS;
}
