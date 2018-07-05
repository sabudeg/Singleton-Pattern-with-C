#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#define NO_OF_THREADS 10

struct Singleton {
   char *Data;	
};

struct Singleton *singletonObjectPointer;

int addresses[NO_OF_THREADS];
sem_t sem;

void *runner(void *params); /* the thread */
struct Singleton *getInstance();

int main()
{
	int i;
	sem_init(&sem,0,1);
	pthread_t threadIds[NO_OF_THREADS];
	

	for (i=0; i < NO_OF_THREADS; i++){
		pthread_create(&threadIds[i], NULL, &runner, (void *)(i));
	} 

	/* Wait until all threads are done */
	for (i=0; i < NO_OF_THREADS; i++){
		pthread_join(threadIds[i], NULL);
	} 
	
	/* Control addresses. All of them should be same */
	int prevAddr=addresses[0];
	for (i=1; i < NO_OF_THREADS; i++){
		if(addresses[i]!=prevAddr){
			printf("Object is created more than once\n");
			return -1;
		}
		prevAddr=addresses[i];
	}
	for (i=0; i < NO_OF_THREADS; i++){
		printf("Singleton Addresses for each thread %x\n",addresses[i]);
	}
	printf("Successful\n");
	return 1;
}

/**
 * The thread will begin control in this function
 */
void *runner(void *params) 
{
	int i = (int)params;
	printf("Thread %d\n",i);
	struct Singleton *s = getInstance();
	addresses[i]=s;
	pthread_exit(0);
}

struct Singleton *getInstance(){
	printf("getInstance is called\n");
	if(singletonObjectPointer == NULL){ //this will check whether the singletonObjectPointer is null. if not, it will skip the block and return value.
		sem_wait(&sem); //if thread see it's value greater than zero, thread will get into block and decrement &sem value by 1
						//if thread see it has value 0, thread will wait until previous thread increase it with sem_post
						if (singletonObjectPointer == NULL){ //check if previous thread has created object when other threads are waiting. if it has already created next coming thread will return the value which previous thread assigned.
				printf("singletonObjectPointer is NULL\n");
				singletonObjectPointer = (struct Singleton *)malloc(sizeof(struct Singleton)); //object is creating.
				printf("---Address of singletonObjectPointer is %x\n",singletonObjectPointer);
				singletonObjectPointer->Data="This is object data";
				}
			sem_post(&sem);	//when thread finishes it's job, it will increment &sem value by 1 so the next thread can get into second if blcok if
			}	
		return singletonObjectPointer;	
		}
