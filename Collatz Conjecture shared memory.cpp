#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>

#define SHSIZE 4096	//the size (in bytes) of shared memory object


int main()
{
	// name of the shared memory object
	const char * name = "OS";

	int n, shm_fd; // the input numbers and the memory files descriptor

	void *ptr; // pointer to shared memory object

	printf("Enter number: ");
	scanf("%i", &n); // taking the input from the user
	pid_t pid;
	pid = fork();
	
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); // create the shared memory object

	ftruncate(shm_fd,SHSIZE); // configure the size of the shared memory object

	ptr = mmap(0, SHSIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0); // memory map the shared memory object


	// child process
	if (pid == 0) {
		while(n > 1){

			if (n % 2 == 0)
			{
				n /= 2;
			}
			else
				n = 3*n+1;

			// write to the shared memory objectd
			*((int*) ptr) = n;
			// increament the ptr by sizeof int since we add integers
			ptr += sizeof(int) ;
		}
	}
	// parent process
	else if (pid > 0) {

		wait(NULL) ; // wait for the child process to terminate

		shm_fd = shm_open(name, O_RDONLY, 0666); // open the shared memory object

		ptr = mmap(0, SHSIZE, PROT_READ, MAP_SHARED, shm_fd, 0); // memory map the shared mmemory object

		// read from the shared memory object
		while (1)
		{
			printf("%i ", *((int *)ptr));
			if(*((int *)ptr) == 1){
				break;
			}
			ptr += sizeof(int) ;
		}
		printf("\n");
		shm_unlink(name); // remove the shared memory object

	}
	return 0;
}