#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define __THINK_TIME   		2      
#define __EATING_TIME     	5       
#define __NR_OF_MEALS   	3      	  
#define __NR_OF_PHILOSOPHERS    5      
#define __SEM_INI_VALUE 	1
#define __BOOL 			int
#define __TRUE 			1
#define __FALSE 		0
#define __SEMAPHORE_STATE 	int
#define __LOCK 			-1
#define __UNLOCK 		1

int getInitializedSemaphore();
void preparePhilosopers(const int* semaphoreID);
void philosopher(const int philosopherID,const int* semaphoreID);
void think(const int* philosopherID);
void eat(const int* philosopherID, const int* semaphoreID);
void grabForks(const int* philosopherID, const int* semaphoreID);
void putForksDown(const int* philosopherID, const int* semaphoreID);
int changeSempahoreState(const int* leftForkID,const int* rightForkID, const int* semaphoreID, const __SEMAPHORE_STATE state );
void finalize(const int* semaphoreID);
