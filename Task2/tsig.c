#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

typedef int bool;
#define true 1
#define false 0

#define __CHILDSLEEP 10
#define __PARENTSLEEP 1
#define	NUM_CHILD	9
#define WITH_SIG

#ifdef WITH_SIG
char __INTERRUPT_FLAG = 0;

void FinilizeChild() {
	printf("Child id : %d has been terminated\n", getpid());
	exit(1);
}
void KbdInterruptHandler() {
	printf("Parent id : %i received keybord interrupt (Ctrl+C)\n", getpid());
	__INTERRUPT_FLAG = 1;
}
#endif // WITH_SIG

void createChild(pid_t* pidCollection, int nChild, int* nCreated)
{
    pid_t pid = fork();
    if(pid == -1){
        fprintf(stderr, "Fork Failed\nKilling remaing child processes\n");
        int i = 0;
        for(; i < nChild; i++)
        {
           kill(pidCollection[i], SIGTERM);
        }
        exit(1);
    } else if(pid > 0) {
        pidCollection[nChild] = pid;
        *nCreated = *nCreated + 1;
    } else if (pid == 0) {
        #ifdef WITH_SIG
            signal(SIGINT,FinilizeChild);
        #endif // WITH_SIG
        printf("-Child number: %d with pid: %d of parent: %d; started \n",nChild, (int)getpid(),(int)getppid());
        printf("falling asleep for %d s \n", __CHILDSLEEP);
        sleep(__CHILDSLEEP);
        printf("-Child number: %d with pid: %d ; finished \n" ,nChild ,(int)getpid());
        exit(0);
    }
}

int childFactory(){
    pid_t pidCollection[NUM_CHILD];
    int i = 0;
    int nCreated = 0;
    bool notInterrupted = true;
	for(; i < NUM_CHILD && notInterrupted; i++){
		createChild(pidCollection, i,&nCreated);
		sleep(__PARENTSLEEP);
		#ifdef WITH_SIG
			if (__INTERRUPT_FLAG == 1){
				printf("\nparent [%i]: Interrupt of the creation process!\n", getpid());
				kill(-2, SIGTERM);
				notInterrupted =  false;
			}
		#endif // WITH_SIG
	}
	if (nCreated == NUM_CHILD){
        printf("\nAll processes have been created.\n");
	}
	return nCreated;
}

void await(pid_t* finishedPidCollection, int* exitCodeCollection, int nCreated)
{
    int child_status;
    int i = 0;
    bool waitErr = false;
	for(; i < nCreated && !waitErr; i++){
		finishedPidCollection[i] = wait(&child_status);
		if(finishedPidCollection[i] == -1)
			waitErr = true;
		else{
			if(WIFEXITED (child_status))
				exitCodeCollection[i] = WEXITSTATUS(child_status);
		}
	}
	printf("\nAll child processes finished.\n");
}

void initialize()
{
    printf("tsig_id PID %d\n", (int) getpid());
    #ifdef WITH_SIG
            int i = 0;
			for(; i < NSIG; i++){
				sigignore(i);
			}
			signal (SIGCHLD, SIG_DFL); //SIG_DFL specifies the default disposition for the signal
			signal (SIGINT, KbdInterruptHandler); //SIG_INT specifies that the signal should be ignored
	#endif // WITH_SIG
}

void finalize(pid_t* finishedPidCollection, int* exitCodeCollection, int nCreated)
{
	printf("\nNumber of finished childs [%d]:  \n", nCreated);
	int i = 0;
	for(; i < nCreated; i++)
		printf("\t[PID]: %d EXIT_CODE: %d\n", finishedPidCollection[i], exitCodeCollection[i]);
	#ifdef WITH_SIG
        int j = 0;
		for(; j<NSIG; j++)
			signal(j, SIG_DFL);
	#endif // WITH_SIGP
}

int main(){
    initialize();
    int nCreated = childFactory();
	pid_t finishedPidCollection[nCreated];
	int exitCodeCollection[nCreated];
    await(finishedPidCollection,exitCodeCollection,nCreated);
    finalize(finishedPidCollection,exitCodeCollection,nCreated);
    return 0;
}
