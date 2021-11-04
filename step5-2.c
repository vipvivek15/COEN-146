//Name: Vivek
//Date: 9/22/2020
//Title: Lab1 - Step 5
//Description: This program uses threading instead of forking
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<pthread.h>

void *start_routine(void *arg)
{
	int i;
	//Parent process
	for(i=0;i<100;i++)
	{
		printf("\n Parent process: %d \n", i);
		usleep(*(int*)arg);
	}
}
/*Main threading function*/
int main(int argc, char *argv[])
{
	pid_t p;
	int i, n = atoi(argv[1]); //n microseconds for delay
	printf("\n Before forking. \n");
	pthread_t t;
	pthread_create(&t,NULL,start_routine,&n);
	// Child process
	for(i=0;i<100;i++)
	{
		printf("Child process %d\n",i);
		usleep(n);
	}
	pthread_join(t,NULL);
	return 0;
}
