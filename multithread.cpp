// multithreading program simple example

#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

using namespace std;
int min, max, avg;

void *mini(void* ptr);
void *maxi(void* ptr);
void *avrg(void* ptr);
int main(int argc, char const *argv[])
{
	short int t1,t2,t3; // to store the return value from creating the thread
	pthread_t thread1, thread2, thread3; // declare threads variable
	//let's say the user will input 7 digits (sample example)
	int arr[7];
	for (int i = 0; i < 7; ++i)
	{
		cin>>arr[i];
	}
	// creating threads
	t1 = pthread_create(&thread1, NULL, mini, (void*) arr);
	t2 = pthread_create(&thread2, NULL, maxi, (void*) arr);
	t3 = pthread_create(&thread3, NULL, avrg, (void*) arr);
	// waiting for the specified thread to terminate
	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL); 
	pthread_join( thread3, NULL); 
	exit(0);
}
void *maxi(void* ptr)
{
	int *ar;
	ar = (int*) ptr;
	int x = ar[0];
	for (int i = 1; i < 7; ++i)
	{
		if (ar[i] > x)
		{
			x = ar[i];
		}
	}
	printf("The maximum value is %i\n", x);
}
void *avrg(void* ptr)
{
	int *ar;
	ar = (int*) ptr;
	int x = 0;
	for (int i = 0; i < 7; ++i)
	{
		x += ar[i];
	}
	printf("The average value is %i\n", x/7);
}

void *mini(void* ptr)
{
	int *ar;
	ar = (int*) ptr;
	int x = ar[0];
	for (int i = 1; i < 7; ++i)
	{
		if (ar[i] < x)
		{
			x = ar[i];
		}
	}
	printf("The minimum value is %i\n", x);
}