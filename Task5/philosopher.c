#include "philosopher.h"

int getInitializedSemaphore()
{
    int semaphoreID , i;

    semaphoreID = semget(IPC_PRIVATE, __NR_OF_PHILOSOPHERS, IPC_CREAT | 0666);
    
    if(semaphoreID == -1){
        perror("Failed to Create Semaphore Aborting");    
        exit(1);
    }
    
    for(i = 0; i < __NR_OF_PHILOSOPHERS; i++){
        if(semctl(semaphoreID, i, SETVAL, __SEM_INI_VALUE) == -1){
            perror("Semaphore initzalitaztion Failed");
            exit(1);
        }
    }
    return semaphoreID;
}

void preparePhilosopers(const int* semaphoreID)
{
	pid_t pid;
	int philosopherID;

	for(philosopherID = 0; philosopherID< __NR_OF_PHILOSOPHERS; philosopherID++){
        pid = fork();
        if(pid < 0){
            kill(-2, SIGTERM);
            perror("Fork failed child process was not created\n");
	    semctl(*semaphoreID, 1, IPC_RMID, NULL);
            exit(1);
        }  
        else if(pid == 0){  
	    philosopher(philosopherID,semaphoreID);
        }
    }	
}

void philosopher(const int philosopherID,const int* semaphoreID)
{

    int mealCnt = 1;
    __BOOL isHungry = __FALSE; 

    printf("Philospoher NR. %d \tjoined the table\n", philosopherID);

    while(mealCnt <= __NR_OF_MEALS){
        if(isHungry){
            eat(&philosopherID, semaphoreID);
    	    printf("Philospoher NR. %d \teat his %d meal\n", philosopherID, mealCnt);
            isHungry = __FALSE;
	    ++mealCnt;    
        }    
        else {
            think(&philosopherID);
            isHungry = __TRUE;
	}
    }   
    printf("Philospoher NR. %d \teat all of meal\n", philosopherID);
    exit(0);
}

void think(const int* philosopherID)
{
    printf("Philospoher NR. %d \thas entered thinking stated \n", *philosopherID);
    sleep(__THINK_TIME);
}

void eat(const int* philosopherID, const int* semaphoreID)
{
    grabForks(philosopherID,semaphoreID);
    printf("Philospoher NR. %d \thas started his meal \n", *philosopherID);
    sleep(__EATING_TIME);
    putForksDown(philosopherID,semaphoreID);
}

void grabForks(const int* philosopherID, const int* semaphoreID)
{ 
    int leftForkID = *philosopherID;
    int rightForkID = (leftForkID == 0 ? __NR_OF_PHILOSOPHERS - 1 : leftForkID-1);
    printf("Philospoher NR. %d \treaches for fork #%d and fork #%d.\n", *philosopherID, leftForkID, rightForkID);
    changeSempahoreState(&leftForkID,&rightForkID,semaphoreID,__LOCK);
}

void putForksDown(const int* philosopherID, const int* semaphoreID)
{
    int leftForkID = *philosopherID;
    int rightForkID = (leftForkID == 0 ? __NR_OF_PHILOSOPHERS - 1 : leftForkID-1);
    printf("Philospoher NR. %d \tstoped using fork #%d and fork #%d.\n",*philosopherID,leftForkID, rightForkID);
    changeSempahoreState(&leftForkID,&rightForkID, semaphoreID,__UNLOCK);
}

int changeSempahoreState(const int* leftForkID,const int* rightForkID, const int* semaphoreID, const __SEMAPHORE_STATE state )
{
    struct sembuf operation[2];
    operation[0].sem_num = *leftForkID;
    operation[0].sem_op = state;
    operation[0].sem_flg = 0;
 
    operation[1].sem_num = *rightForkID;
    operation[1].sem_op = state;
    operation[1].sem_flg = 0;

    return semop(*semaphoreID, operation, 2);
}

void finalize(const int* semaphoreID)
{
    int child_status;
    pid_t pid;

    __BOOL hasWaitingProcesses = __TRUE;
    while(hasWaitingProcesses){
        pid = wait(&child_status);
        if(pid < 0)
            hasWaitingProcesses = __FALSE;    
    }   

    printf("DINER ENDED\n");
    semctl(*semaphoreID, 1, IPC_RMID, NULL);
}


int main(){
    int semaphoreID;

    semaphoreID = getInitializedSemaphore();
    preparePhilosopers(&semaphoreID);
    finalize(&semaphoreID);    

    return 0;
}
