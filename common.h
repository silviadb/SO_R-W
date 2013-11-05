#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
 
#define PERM 0666
 
#define NSEM 3
 
#define MUTEXSEM 0
#define EMPTYSEM 1
#define FULLSEM 2
 
static struct sembuf decEmpty = { EMPTYSEM, -1, 0 };
static struct sembuf incEmpty = { EMPTYSEM, 1, 0 };
static struct sembuf incFull = { FULLSEM, 1, 0 };
static struct sembuf decFull = { FULLSEM, -1, 0 };
static struct sembuf incMutex = { MUTEXSEM, 1, 0 };
static struct sembuf decMutex = { MUTEXSEM, -1, 0 };
 
struct product {
 char name [100];
 char code;
};
 
struct ringBuffer
{
 char in,out,read,isOpen,size;
};

struct hilo_rw
{
 int s_key, m_key ,sleep, num_p;
};
