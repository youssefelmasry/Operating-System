#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <time.h>

#define N 5 // number of philosphers
#define LEFT (i+N -1)%N // number of i's left neighbor
#define RIGHT (i+1)%N // number of i's right neighnor
#define THINKING 0 // philosopher is thinking
#define HUNGRY 1 // philsopher is trying to get forks
#define EATING 2 // philospher is eating

pthread_mutex_t lock;
pthread_cond_t forks_condition[N]; // one condition per philosopher initialized to 0
int state[N]; //array to keep track of everyone’s state


void *test(int); // declare test funtion
void *philosopher(void* i);
void *take_forks(int i);
void *put_forks(int i);
void think();
void eat();
void delay(unsigned int mseconds);
void print_info();
void *info();


int main()
{
	int t=0,i;
	pthread_t tid[N];
	pthread_t infot;
	const char *names[] = {"0","1","2","3","4"};
	// initialize the mutex
	if(pthread_mutex_init(&lock,NULL) != 0)
	{
		printf("\nmutex init failed\n");
		exit(1);
	}

	// creatng philosopher threads
	for(i = 0; i<5; i++)
	{
		t = pthread_create(&tid[i],NULL, philosopher,  (char *) names[i]);
		//pthread_join(tid[i],NULL);
		if(t != 0)
		{
			printf("\n Error in creating thread");
			exit(1);
		}
	}
	// create information threads about the current eating philosopher
	pthread_create(&infot,NULL,info,NULL);

	//join philosopher threads
	for(i=0;i<5;i++)
	{
		t=pthread_join(tid[i],NULL);
		if(t!=0)
		{
			printf("\n Joining thread Failed");
			exit(1);
		}
	}

	// join information thread
	pthread_join(infot,NULL);

	//destroy the mutex
	pthread_mutex_destroy(&lock);
	return 0;
}

void print_info()
{
	int n = 0, a[2];
	pthread_mutex_lock(&lock);
	for(int i = 0; i<5; i++)
	{
		if(state[i] == EATING)
			a[n++] = i;
	}
	pthread_mutex_unlock(&lock);
	printf("# of philosopher eating is %d: ", n);
	for(int k = 0; k<n; k++)
	{
		printf("%d ", a[k]);
		if(k<n-1)
			printf(", ");
	}
	printf("\n");
}

void *info()
{
	int c = 0;
	while(c<50){
		delay(1000);
		print_info();
		c++;
	}
}
/*check if the philosopher is HUNGRY and the left and the right one is not eating*/
void *test(int i)
{
	if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
		{
			state[i] = EATING;
			pthread_cond_signal(&forks_condition[i]);
		}		
}
/*switch the state of philosopher to HUNGRY, and test philosopher to eat,
If yes, change his state to EATING. If not block until a another philosopher put down its forks*/
void *take_forks(int i) // i: philosopher number, from 0 to N-1(4)
{
	pthread_mutex_lock(&lock);
	state[i] = HUNGRY;
	test(i);
	while(state[i] == HUNGRY){
		pthread_cond_wait(&forks_condition[i], &lock);
	}
	pthread_mutex_unlock(&lock);
}
/*switch the state of philosopher to THINKING
and check if the left and the right one can eat, If yes, change their state to EATING*/
void *put_forks(int i)
{
	pthread_mutex_lock(&lock);
	state[i] = THINKING;
	test(LEFT);
	test(RIGHT);
	pthread_mutex_unlock(&lock); // exit critical section
}

void delay(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

void think()
{
	delay(1000);
}
void eat()

	delay(1000);
}
void *philosopher(void *i)
{
	int c = 0;
	int h = atoi((char*) i);
	printf("\nPhilosopher %d \n", h);
	while(c<50)
	{
		think();
		take_forks(h);
		eat();
		put_forks(h);
		c++;
	}
}