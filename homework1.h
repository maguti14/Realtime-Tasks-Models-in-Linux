#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <asm/unistd_64.h>
#include <regex.h>
#include <sys/types.h>
#include <signal.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sched.h>

#define BUFFSIZE 2048
#define BILL 1000000000
#define MTON 1000000
/* ================================== */
/* -------- Global Variables -------- */
pthread_t *thread_ptr;
int counter = 0;
int numOfTasks = 0;
pthread_cond_t condition;
pthread_cond_t event0Cond;
pthread_cond_t event1Cond;
pthread_cond_t timeCond;
pthread_mutex_t condition_mutex;
pthread_mutex_t event0Cond_mutex;
pthread_mutex_t event1Cond_mutex;
pthread_mutex_t timeMutex;
int execTime = 0;
int runCondition = 1;//place holder depending how we want to do exit.
struct timespec startTime, endTime;
pthread_mutexattr_t inherit;
pthread_mutex_t mutex0;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;
pthread_mutex_t mutex4;
pthread_mutex_t mutex5;
pthread_mutex_t mutex6;
pthread_mutex_t mutex7;
pthread_mutex_t mutex8;
pthread_mutex_t mutex9;
/* ------ End Global Variables ------ */
/* ================================== */


/* ================================== */
/* -------- Helper Functions -------- */
char* readline() {
	char* buffer = (char*)malloc(BUFFSIZE * sizeof(char));
	fgets(buffer, BUFFSIZE, stdin);
	int buff_len = strlen(buffer);
	char* line = (char*)malloc(buff_len * sizeof(char));

	for(int i = 0; i < strlen(buffer)-1; i++) {
        line[i] = buffer[i]; 
    }
    line[strlen(buffer)-1] = '\0';

	free(buffer);
	return line;
}

int char_to_int(char c) {
	if(!isdigit(c)){
		return -1;
	}
	int x = c - '0';
	return x;
}

int str_to_int(char* str) {
	int result = 0;
	if((strlen(str) == 2) && (str[strlen(str)-1] == '\0')) {		//2 because it will include the null terminator
		//case there's only a character
		result = char_to_int(str[0]);
	}
	else {
		//case more than 1 char
		for(int i = 0; i < strlen(str);i++){
			int x = char_to_int(str[i]);
			if(x == -1){
				printf("Error\n");
				return -1;
			}
			result = result * 10 + x;
		}
	}
	return result;
}

/* ------ End Helper Functions ------ */
/* ================================== */




/* ================================== */
/* ----------- Functions  ----------- */

//correct way to add our period to the time structs
void addPeriod(struct timespec *start, struct timespec *end, int periodIn)
{
	long exec = (long) periodIn; // assuming input is type int
	long sec = exec/1000; 
	long nsec = (exec - sec*1000)*MTON;
	end->tv_sec = start->tv_sec + sec;
	end->tv_nsec = start->tv_nsec + nsec;
	if(end->tv_nsec >= BILL)
	{
		end->tv_nsec -= BILL;
		end->tv_sec++;
	}
}

// handler for periodic threads waiting on nano_sleep which will be interrrupted upon gettign a signal so we can use it to kill a waiting thread instantly.
void killOnWait(int sig)
{
	printf("Waking up task\n");
	//pthread_exit(NULL);
	runCondition = 0;	
}

// handler will exit immediately anyway
void killHandler()
{
	printf("Waking up Handler\n");
	pthread_exit(NULL);
}


pthread_mutex_t* which_mutex(char* str){
	pthread_mutex_t* mutex = NULL;
	if(str[1] == 0)
		mutex = &mutex0;
	else if(str[1] == '1')
		mutex = &mutex1;
	else if(str[1] == '2')
		mutex = &mutex2;
	else if(str[1] == '3')
		mutex = &mutex3;
	else if(str[1] == '4')
		mutex = &mutex4;
	else if(str[1] == '5')
		mutex = &mutex5;
	else if(str[1] == '6')
		mutex = &mutex6;
	else if(str[1] == '7')
		mutex = &mutex7;
	else if(str[1] == '8')
		mutex = &mutex8;
	else if(str[1] == '9')
		mutex = &mutex9;
	else 
		printf("Error in which_mutex");

	return mutex;
}

// our computation
void compute(int x){
	int i, j = 0;
	for(i = 0; i < x; i++){
		j = j + i;
	}
}

// critical section uses the input string to determine actions to take
void CS(char* orig_str) {

	int reti;
	regex_t lockRex;
	/* Compile regular expression */
	reti = regcomp(&lockRex, "L[0-9]", 0);
	if (reti) {
	    exit(1); //fprintf(stderr, "Could not compile lockRex\n");
	}
	regex_t unlockRex;
	/* Compile regular expression */
	reti = regcomp(&unlockRex, "U[0-9]", 0);
	if (reti) {
	    exit(1); //fprintf(stderr, "Could not compile unlockRex\n");
	}
	
	char* tok1 = strtok_r(orig_str," ",&orig_str);
	if(tok1 == NULL){
		return;
	}

	/* Matching lockRex with tok1 */
	reti = regexec(&lockRex, tok1, 0, NULL, 0);
	if (!reti) {
	   
	    int mutex_lock_check = pthread_mutex_lock(which_mutex(tok1));
	    if(mutex_lock_check != 0){
	    	printf("mutex did not lock");
	    }
	    
	    char* tok = strtok_r(orig_str," ",&orig_str);
	    int itr = str_to_int(tok);
	    compute(itr);
	    char* new_str = strdup(orig_str);
	    CS(new_str);
	}

	/* Matching lockRex with tok1 */
	reti = regexec(&unlockRex, tok1, 0, NULL, 0);
	if (!reti) {
	   
	   	int mutex_unlock_check = pthread_mutex_unlock(which_mutex(tok1));
	   	if(mutex_unlock_check != 0){
	    	printf("mutex did not unlock");
	    }
	   
	    char* tok = strtok_r(orig_str," ",&orig_str);
	    int itr = str_to_int(tok);
	   
	    compute(itr);
	    char* new_str = strdup(orig_str);
	    
	    CS(new_str);
	}
}

void task_body(char* saveptr){
	char* tok1 = strtok_r(saveptr," ",&saveptr);
	char* orig_str = strdup(saveptr);
	char* tok2 = strtok_r(saveptr," ",&saveptr);
	//printf("tok1 = %s\n",tok1);
	//printf("tok2 = %s\n",tok2);
	
	// Two cases: 
	if(tok2 == NULL) {	
		// Case 1: no critical sections
		int itr = str_to_int(tok1);
		compute(itr);

	}
	else {
		// Case 2: critical sections
		int itr = str_to_int(tok1);
		compute(itr);
		CS(orig_str);
	}
}

void wait_for_activation(){
	// Barrier implementation from: http://cs.umw.edu/~finlayson/class/fall14/cpsc425/notes/07-conditions.html
	/***************** begin barrier here *****************/
	/* lock the condition and increment the counter */
	//printf("counter = %d\n",counter);
	pthread_mutex_lock(&condition_mutex);
	//printf("COunter = %d, number of tasks = %d\n", counter, numOfTasks);

	/* if all the threads have incremented counter */
	if (counter == numOfTasks+1)
	{
		//printf("counter = numOfTasks = %d\n",counter);
		/* reset the counter and broadcast the event to all waiting threads */
		counter = 0;
		clock_gettime(CLOCK_MONOTONIC, &startTime); // start count
		addPeriod(&startTime, &endTime, execTime);
		pthread_cond_broadcast(&condition);
	} 
	else 
	{
		/* otherwise not all threads are here yet, so we need to wait */
		counter++;
		pthread_cond_wait(&condition, &condition_mutex);
	}
	/* unlock the condition mutex */
	pthread_mutex_unlock(&condition_mutex);
	/***************** end barrier here *****************/
}

void wait_for_left_mouse(int trigger){
	//printf("[DEBUG] wait_for_left_mouse\n");
	pthread_mutex_lock(&event0Cond_mutex);
	
	if (trigger == 1) //mouse event broadcast wake up
	{
		pthread_cond_broadcast(&event0Cond);
	} 
	else // 0 - task waiting for event
	{
		pthread_cond_wait(&event0Cond, &event0Cond_mutex);
	}
	pthread_mutex_unlock(&event0Cond_mutex);
}

void wait_for_right_mouse(int trigger){
	//printf("[DEBUG] wait_for_right_mouse\n");
	pthread_mutex_lock(&event1Cond_mutex);
	
	if (trigger == 1) //mouse event broadcast wake up
	{
		pthread_cond_broadcast(&event1Cond);
	} 
	else // 0 - task waiting for event
	{
		pthread_cond_wait(&event1Cond, &event1Cond_mutex);
	}
	pthread_mutex_unlock(&event1Cond_mutex);
}

void *eventHandler()
{
	pthread_t tid = pthread_self();
	struct sched_param param;
	int policy = 0;
	pthread_getschedparam(tid,&policy,&param);
	printf("Handler priority is: %d, policy: %d, SCHED_FIFO: %d\n",param.sched_priority,policy,SCHED_FIFO);

	wait_for_activation();
	int trigger = 1;
	
	signal(SIGINT, killHandler);
	int file = open("/dev/input/event2", O_RDONLY); // event2 is the usb for the mouse on the board so we have configured our host as such as well
	if (file == 0)
	{
		fprintf(stderr, "Could not find Mouse in event2 file\n");
		return NULL;
	}
	
	struct input_event event; // struct for the event input 
	while(runCondition)
	{	
		read(file, &event, sizeof(event));
		
		//only trigger on:
		// event 0 = release left mouse: event.code == BTN_LEFT && event.value == 0
		// event 1 = release right mouse: event.code == BTN_RIGHT && event.value == 0
		if(event.code == BTN_LEFT && event.value == 0)
		{
			wait_for_left_mouse(trigger);
		}
		else if(event.code == BTN_RIGHT && event.value == 0)
		{
			wait_for_right_mouse(trigger);
		}
	}
	pthread_exit(NULL);
	return NULL;
}

void *execTimer()
{
	pthread_t tid = pthread_self();
	struct sched_param param;
	int policy = 0;
	pthread_getschedparam(tid,&policy,&param);
	printf("Timer priority is: %d, policy: %d, SCHED_FIFO: %d\n",param.sched_priority,policy,SCHED_FIFO);
	
	wait_for_activation();

	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &endTime, 0);
	runCondition = 0; // exit! clear runCondition before signaling other tasks to exit to avoid race-conditions.

	for(int i = 1; i <= numOfTasks; i++) //signal waiting threads to exit immediately
	{
		pthread_kill(thread_ptr[i],SIGUSR1);
	}

	pthread_kill(thread_ptr[numOfTasks + 1],SIGINT);
	wait_for_left_mouse(1);
	wait_for_right_mouse(1);
	pthread_exit(NULL);
	return NULL;
}

// periodic tasks will exit immediately if they are waiting because of the way clock_nanosleep is interrupted by signals.
// However, if it is working it will finish working and then check the runCondition to exit the while loop and terminate gracefully.
void *periodic_task(void *ptr) {

	//< local variables > and initialization()
	pthread_t tid = pthread_self();
	struct sched_param param;
	int policy = 0;
	pthread_getschedparam(tid,&policy,&param);
	

	signal(SIGUSR1, killOnWait);
	
	char* saveptr = (char*) ptr;

	char* tok = strtok_r(saveptr," ",&saveptr);
	int timePeriod = str_to_int(tok);

	struct timespec next, end;
	wait_for_activation();
	
	printf("Periodic priority is: %d, policy: %d, SCHED_FIFO: %d\n",param.sched_priority,policy,SCHED_FIFO);
	while (runCondition) {
		clock_gettime(CLOCK_MONOTONIC, &next);
		char* new_str = strdup(saveptr);
		task_body(new_str);
		if(!runCondition)
			break; // check flag after task is done
		addPeriod(&next, &end, timePeriod);
		// if the given time to wake up has passed (task overran) clock wil not sleep - overrun handled!
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &end, 0);

		free(new_str);
	}	
	pthread_exit(NULL);
	return NULL;
}

// aperiodic tasks will exit immediately if they are waiting because the timer will broadcast all mouse events they are waiting on
// However, if it is working it will finish working and then check the runCondition to exit the while loop and terminate gracefully.
void *aperiodic_task(void *ptr) {

	//< local variables > and initialization()
	pthread_t tid = pthread_self();
	struct sched_param param;
	int policy = 0;
	pthread_getschedparam(tid,&policy,&param);
	
	
	char* saveptr = (char*) ptr;
	
	char* tok = strtok_r(saveptr," ",&saveptr);
	int trig_event = str_to_int(tok);
	

	wait_for_activation();
	printf("Aperiodic priority is: %d, policy: %d, SCHED_FIFO: %d\n",param.sched_priority,policy,SCHED_FIFO);
	
	while (runCondition) {
		char* new_str = strdup(saveptr);
		task_body(new_str);
		if(!runCondition)
			break; // check after task is done
		if(trig_event == 0) {
			wait_for_left_mouse(0); // wait on the event as a condition
		}
		else {// only other option is 1 - assumed to be correct input format
			wait_for_right_mouse(0);
		}
		free(new_str);
	}
	pthread_exit(NULL);
	return NULL;
}
/* --------- End Functions  --------- */
/* ================================== */
