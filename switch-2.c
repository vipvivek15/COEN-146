// Name: Vivek Ponnala
// Date: 9/22/2020
// Title: Lab1 -Circuit Switching
// Description: Steps 6 and 7 for Lab 1. 
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*Create a function to represent the binomial of an integer*/
int binom(int a,int b)
{
	return (b==0||b==a) ? 1 : binom(a-1,b-1)+binom(a-1,b);
}
int main(int argc, char *argv[])
{
/*
 	Variables used in computation:
	int linkBandwidth; //bandwidth of network link is denoted by int linkBandwidth
	int userBandwidth; //bandwidth for given user
	number of circuit switching users; int nCSusers;
	number of packet switching users: int nPSusers;
	Percentage of time packet switching user needs to transmit is double tPSuser;
	Probability that a given packet switching user is busy transmitting is double pPSusersBusy
	Probability that one packet switching user is not busy transmitting is double pPSuserNotBusy
*/
	int linkBandwidth=atoi(argv[1]);
	int userBandwidth=atoi(argv[2]);
	double tPSuser=atof(argv[3]);
	int nPSusers=atof(argv[4]);

//Part 6
	int nCSusers=linkBandwidth/userBandwidth;
	printf("6a -Number of circuit switching users is: %d\n",nCSusers);
//Part 7 a
	double pPSusers =tPSuser;
	printf("7a- The probability of a packet switching user is: %.2lf\n",pPSusers);
//Part 7 b	
	double pPSusersNotBusy =1-pPSusers;
	printf("7b- The probability of packet switching user not being busy is: %.2lf\n",pPSusersNotBusy);
//Part 7 c
	double allusers=pow((1-pPSusers),(nPSusers-1));
	printf("7c-Probability of all users not busy:%e\n",allusers);
//Part 7 d
	double one=pPSusers*pow(pPSusersNotBusy,(nPSusers-1));
	printf("7d-Probability of one user busy:%e\n",one);
//Part 7 e
	double exactone=nPSusers*one;
	printf("7e-Probability of one user:%e\n",exactone);
//Part 7 f
	double ten=pow(pPSusers,10)*pow(pPSusersNotBusy,(nPSusers-10));
	printf("7f-Probability of exactly ten:%e\n",ten);
//Part 7 g
	double anyten=binom(nPSusers,10)*ten;
	printf("7g-Probability of any 10 users:%e\n",anyten);
//Part 7 h
	double morethanten=0;
	int i;
	for(i=11;i<=nPSusers;i++)
	{
		morethanten+=binom(nPSusers,i)*pow(pPSusers,i)*pow(pPSusersNotBusy,(nPSusers-i));
	}
	printf("7h-Probability that more than ten users are transmitting:%e",morethanten);
return 0;
}
