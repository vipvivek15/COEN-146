/*
 Name:Vivek Ponnala
 Date:9/29/2020
 Title: Lab #2 Part 2 -Computing the connection time with server
 Description-Compute times listes in COEN 146
*/

  #include<stdio.h>
  #include<stdlib.h>
  #include<math.h>

  #define RTT1 3
  #define RTT2 20
  #define RTT3 26
  #define RTTHTTP 47
  #define objects_used 6

int main()
{
		//part a
		int c_r = RTT1+RTT2+RTT3+2*RTTHTTP;
		printf("Time elapsed till client clicks: %d milliseconds\n",c_r);
		
		//part b: non persistent and parallel connections
		int nopar = c_r + (objects_used*2*RTTHTTP);
		printf("Time elapsed while referencing 6 small objects: %d milliseconds\n",nopar);
		
		//part c: 
		int c=5; // parallel connections
		
		//non persistent case:
		int non_pers = c_r + (2*ceil(objects_used/c)*RTTHTTP);
		printf("Time elapsed for non-persistent case:%d msec\n",non_pers);
		int pers = c_r + (ceil(objects_used/c)*RTTHTTP);
                printf("Time elapsed for persistent case:%d msec\n",pers);	
		
		return 0;
}
