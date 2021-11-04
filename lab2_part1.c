/*
Name:Vivek Ponnala
Date: 9/29/2020
Title: Part 1-Multithreading in c
Description: Using parallelism
*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>

#define N 1024
#define M 1024
#define L 1024

pthread_t threads[N];

double A[N][M], B[M][L], C[N][L] = {0.0};

void show(int r, int c, double Matrix[r][c]);

void *start_routine(void *arg)
{
	int b, c=0; // contains the values of L and M
	int a= *(int*) arg; //constains the values of N
	for(b=0;b<L;b++)
	{
		for(c=0;c<M;c++)
		{
			C[a][b]+=A[a][c]*B[c][b];
		}
	}
	free(arg); //free the memory associated with arg which contains the values of N			
		
}
int main()
{
	srand(time(NULL));
	int i, j=0;
	// obtain matrix A
	for(i=0;i<N;i++)
	{
		for(j=0;j<M;j++)
		{
			A[i][j]=rand();
		}
	}
	//obtain matrix B
	for(i=0;i<M;i++)
        {
                for(j=0;j<L;j++)
                {
                        B[i][j]=rand();
                }
        }
	//matrix C by computing A X B
	for(i=0;i<N;i++)
	{
		int *arg = malloc(sizeof(int));
		*arg=i;
		pthread_create(&threads[i],NULL,start_routine,arg);
	}
	printf("Matrix A:\n");
	show(N,M,A);
	printf("Matrix B:\n");
	show(M,L,B);
	printf("Matrix C:\n");
	show (N,L,C);

	return 0;
}	
void show(int r, int c, double Matrix[r][c])
{
	int i, j=0;
	for(i=0;i<r;i++)
	{
		for(j=0;j<c;j++)
		{
			printf("%lf",Matrix[i][j]);
		}
		printf("\n");
	}
}	
	

