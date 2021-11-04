// Name: <Vivek>
// Date: <9/22/20>
// Title: Lab1 - task
// Description:This is a sample program of fork operation.
#include<stdio.h> /*printf,stderr */
#include<sys/types.h> /* pid_t */
#include<unistd.h> /*fork */
#include<stdlib.h> /*atoi*/
#include<errno.h> /* erno */
/* main function with command-line arguments to pass */
int main(int argc, char *argv[])
{
	pid_t pid;
	int i,n=atoi(argv[1]); // n microseconds to input from keyboard for delay
	printf("Before forking\n");
	pid=fork();
	if(pid==-1)
	{
		fprintf(stderr, "can't fork error %d\n", errno);
	}
	if(pid)
	{
		// Parent process
		for(i=0;i<100;i++)
		{
			printf("Parent process %d\n",i);
			usleep(n);
		}
	}
	else
	{
		//Child process
		for(i=0;i<100;i++)
		{
			printf("Child process %d\n",i);
			usleep(n);
		}
	}
	return 0;
}
