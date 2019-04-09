#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "homework1.h"
#define BUFFSIZE 2048

int main(int argc, char* argv[]) {

	if(argc != 2)
	{
		printf("Correct ussage is: %s [PI] < <filename>\n Where Pi can be 0 = disabled or 1 = enabled\n", argv[0]);
		exit(0);
	}
	
	//allow PI enable/dissable as a command line argument for ease of testing
	if(atoi(argv[1]) == 1)
	{
		printf("PI enabled\n");
		pthread_mutexattr_setprotocol(&inherit, PTHREAD_PRIO_INHERIT);
		pthread_mutex_init(&mutex0, &inherit);
		pthread_mutex_init(&mutex1, &inherit);
		pthread_mutex_init(&mutex2, &inherit);
		pthread_mutex_init(&mutex3, &inherit);
		pthread_mutex_init(&mutex4, &inherit);
		pthread_mutex_init(&mutex5, &inherit);
		pthread_mutex_init(&mutex6, &inherit);
		pthread_mutex_init(&mutex7, &inherit);
		pthread_mutex_init(&mutex8, &inherit);
		pthread_mutex_init(&mutex9, &inherit);
	}
	else // == 0
	{
		printf("No PI\n");
		pthread_mutexattr_setprotocol(&inherit, PTHREAD_PRIO_NONE);
		pthread_mutex_init(&mutex0, &inherit);
		pthread_mutex_init(&mutex1, &inherit);
		pthread_mutex_init(&mutex2, &inherit);
		pthread_mutex_init(&mutex3, &inherit);
		pthread_mutex_init(&mutex4, &inherit);
		pthread_mutex_init(&mutex5, &inherit);
		pthread_mutex_init(&mutex6, &inherit);
		pthread_mutex_init(&mutex7, &inherit);
		pthread_mutex_init(&mutex8, &inherit);
		pthread_mutex_init(&mutex9, &inherit);
	}

	char* line1 = readline();
	char* firstPart = strtok(line1," ");
	
	numOfTasks = str_to_int(firstPart);										// check for error if needed
	
	char* strExecTime = strtok(NULL,"\n");
	execTime = str_to_int(strExecTime);

	
	thread_ptr = (pthread_t*)malloc((numOfTasks+2) * sizeof(pthread_t)); 	//create as many threads as numOfTasks + execTime + listener
	
	pthread_attr_t attr;
	struct sched_param p;
	pthread_attr_init(&attr);												//initialize thread attribute
	pthread_attr_setschedpolicy(&attr,SCHED_FIFO);							//set schedule here
	p.sched_priority = 99;													// handler and timer will have highest priority
	pthread_attr_setschedparam (&attr, &p);									// set parameter
	pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);			// tell it to use this NOT inherit the main's priority by default! Requires SUDO!!!!!!!!!
	
	pthread_create(&thread_ptr[0], &attr, &execTimer, NULL);				//create execTimer thread
	pthread_create(&thread_ptr[numOfTasks+1], &attr, &eventHandler, NULL);	//create eventHandler thread

	//begin parser - we will be passing the input string as needed
	for(int i = 1; i <= numOfTasks; i++) 
	{
		pthread_attr_t tattr;
		struct sched_param param;

		char* line = readline(); 
		char* saveptr = NULL; 
		char* tok = strtok_r(line," ",&saveptr);

		if(tok[0] == 'A') {

			tok = strtok_r(saveptr," ",&saveptr);
			int priority = str_to_int(tok);
			
			//create thread here
			pthread_attr_init(&tattr);											//initialize thread attribute
			pthread_attr_setschedpolicy(&tattr,SCHED_FIFO);							//set schedule here
			param.sched_priority = priority;											
			pthread_attr_setschedparam (&tattr, &param);							//set priority here
			pthread_attr_setinheritsched (&tattr, PTHREAD_EXPLICIT_SCHED);
			char *task_args = strdup(saveptr);

			pthread_create(&thread_ptr[i],&tattr, &aperiodic_task, (void *) task_args);	//create THREAD here!!!!
		}
		else if(tok[0] == 'P') {

			tok = strtok_r(saveptr," ",&saveptr);

			int priority = str_to_int(tok);
			
			//create thread here
			pthread_attr_init(&tattr);											//initialize thread attribute
			pthread_attr_setschedpolicy(&tattr,SCHED_FIFO);							//set schedule here
			param.sched_priority = priority;											
			pthread_attr_setschedparam (&tattr, &param);							//set priority here
			pthread_attr_setinheritsched (&tattr, PTHREAD_EXPLICIT_SCHED);
			char *task_args = strdup(saveptr);

			pthread_create(&thread_ptr[i],&tattr, &periodic_task, (void *) task_args);	//create THREAD here!!!!
		}
		else {
			printf("Input error");
			exit(1);
		}
		
		free(line);
	}
	
	// clean up
	for(int i = 0; i < (numOfTasks+2); i++){
		pthread_join(thread_ptr[i],NULL);
	}

	free(line1);
	printf("\n");
	return 0;
}
