#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
int main()
{
	int i,n;
	std::cin>>n;
	pid_t pid;
	pid = fork();
	
	if (pid == 0) {
		printf("%d, ", n);
		while(n > 1){
			if (n % 2 == 0)
			{
				n /= 2;
				printf("%d, ", n);
			}
			else{
				n = 3*n+1;
				printf("%d, ", n);}
		}
	}
	else if (pid > 0) {
		wait(NULL) ;

	}
	return 0;
}