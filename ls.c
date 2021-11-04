/*
   Name:Vivek Ponnala
   COEN 146 Lab7
   Date: 11/12/2020
   Description: Implementing LS routing using Djkistra's Algorithm
*/
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <limits.h>
#include <time.h>
//DEFINE constants
#define N 4
/*
#define N 6 //Test case
*/
#define INFINITE 1000

#define update_min_wait 10
#define update_max_wait 20

//machine struct used to scan name, ip and port
typedef struct machines
{
	// student work
	char name[50];
	char ip[50];
	int port;
}MACHINE;

//global variables
MACHINE	machines[N]; // array of machine struct used to scan for name, ip and port
int costs[N][N]; //cost array used to obtain costs
int distances[N]; //distances array
int myid, nodes, myport; //global variables
int sock;
int in_data[3];
int out_data[3];
pthread_mutex_t lock;
pthread_t thr1, thr3;
//thread functions
void *run_link_state(void *);
void update(void );
void *receive_info(void *);
void print_costs(void);
//helper functions
void send_data(void);
int receive_data(int);
int minDistance(int [], int []);
//main
int main(int argc, char *argv[])
{
	
	int i, j;

	if (argc < 5)
	{
		fprintf(stderr,"Usage: %s <id> <n_machines> <costs_file> <hosts_file> \n", argv[0]);
		printf("Please only enter compiled file id number number of machiens cost file and host file. Thank you.\n");
		return 1;
	}
	myid = atoi(argv[1]);
	nodes= atoi(argv[2]);

	printf("My id is: %d\n",myid);
	printf("Number of nodes used is: %d\n",nodes);

	//myid should not be greater than 4 and nodes should always be equal to N
	if ( myid >= N)
	{
		printf ("Wrong id\n");
		printf("ID can only be in the range from 0-3\n");
		return 1;
	}
	
	if (nodes != N)
	{
		printf("Wrong number of nodes\n");
		printf("Please enter only %d nodes.\n",N);
		return 1;
	}

	//get info on machines from the file using fopen and fscanf and pupulate the array of machine-struct
	FILE *f_costs;
	f_costs = fopen(argv[3], "r");
	FILE *f_hosts;
	f_hosts = fopen(argv[4], "r");
	
	//ensure file pointers are not null
	if(f_costs==NULL||f_hosts==NULL)
	{
		printf("Ensure cost and machine file are not empty.\n");
		printf("Please enter cost and machine file.\n");
		return 1;
	} 
	
	//parse the file pointers
	printf("Parsing cost file: \n");
        for (i = 0; i<N; i++)
	{
                for (j = 0; j<N; j++)
		{
                        if ( (fscanf(f_costs, "%d", &costs[i][j])) != 1)
			{
                                break;
                        }
                        printf("%d ", costs[i][j]);
                }
                printf("\n");
        }
        printf("Parsing hosts(machines) file:\n");
        for (i = 0; i<N; i++)
	{
	
                if ((fscanf(f_hosts, "%s %s  %d", &(machines[i].name), &(machines[i].ip), &(machines[i].port))) <1)
		{
                        break;
                }
                printf("%s %s %d \n", (machines[i].name), (machines[i].ip), (machines[i].port));
        }
	fclose(f_costs);
	fclose(f_hosts);
	
	myport = machines[myid].port;
	printf("Myport is: %d\n", myport);

	//initialize the  address
 	struct sockaddr_in addr;
	struct sockaddr_in otheradder;
	struct sockaddr_storage myStorage;
	socklen_t addr_size;
	socklen_t other_addr_size;

	//address family, port, size
	addr.sin_family = AF_INET;
	addr.sin_port =  htons((short)myport);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset ((char *)addr.sin_zero, '\0', sizeof (addr.sin_zero));
	addr_size = sizeof(myStorage);

	//create a socket
	
	if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) <0){
		printf("socket error\n");
		return 1;
	}
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0){
		printf("bind error\n");
		return 1;
	}

	//initialize mutex lock
	pthread_mutex_init (&lock, NULL);

	//create threads
	pthread_create (&thr1, NULL, receive_info, NULL);
	pthread_create (&thr3, NULL, run_link_state,NULL);


	for(j=0;j<2;j++)
	{
		update();
		sleep(10);
	}
	return 0;	
}

//thread 1
void *receive_info(void *arg)
{
	
	while(1)
	{
		receive_data(myport);

		int host1 = ntohl(in_data[0]);
		int host2 = ntohl(in_data[1]);
		int weight = ntohl(in_data[2]);

		pthread_mutex_lock(&lock);
		costs[host1][host2] = weight;
		costs[host2][host1] = weight;
		print_costs();
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}
//thread 2 helper function 
void update(void)
{

        int neighbor;
        int new_cost;
        printf("Please update neighbor cost from node %d, format <neighbor> <new cost> :\n", myid);
        scanf("%d %d", &neighbor, &new_cost);
        printf("Values which will be changed in the cost matrix are: <%d><%d> and <%d><%d> with cost %d. This will be updated with Djkistra's algorithm.\n",myid,neighbor,neighbor,myid,new_cost);
        pthread_mutex_lock(&lock);
        costs[myid][neighbor] = new_cost;
        costs[neighbor][myid] = new_cost;
        out_data[0] = htonl(myid);
        out_data[1] = htonl(neighbor);
        out_data[2] = htonl(new_cost);
        send_data();

        printf("New matrix after user input: \n");
        print_costs();
        pthread_mutex_unlock(&lock);
}

//thread 3
void *run_link_state(void *arg)
{

	
	time_t last_update;
	last_update = time(NULL);

	while (1)
	{
		int threshold = rand()%(update_max_wait - update_min_wait) + update_min_wait;
		if ((time(NULL) - last_update) > threshold)
		{
			int dist[N];
			int visited[N];
			int tmp_costs[N][N];
			int i, source;
			pthread_mutex_lock(&lock);
			for (source = 0; source <N; source++)
			{
				for (i = 0; i <N; i++){
					dist[i] = INT_MAX, visited[i] = 0;
				}
				dist[source] = 0;

				int count;
				for (count = 0; count < N-1; count++)
				{
					int u = minDistance(dist, visited);
					visited[u] = 1;
					int v;
					for (v = 0; v <N; v++){
						if (visited[v] == 0 && (costs[u][v] && dist[u]) != INT_MAX && dist[u] + costs[u][v] < dist[v])
						{
							dist[v] = dist[u] + costs[u][v];
						}
					}

					printf("Distances computed in dijkstra from node %d: ", source);
					for (i = 0; i<N; i++){
						printf("%d ", dist[i]);
						tmp_costs[source][i] = dist[i];
						tmp_costs[i][source] = dist[i];
					}
					printf("\n");
				}
				printf("\n");
				pthread_mutex_unlock(&lock);
				last_update = time(NULL);
			}
		}
	}
	return NULL;
}

int minDistance(int dist[], int visited[])
{
	int min = INT_MAX, min_index;
	int v;
	for (v = 0; v<N; v++){
		if (visited[v] == 0 && dist[v] < min){
			min = dist[v], min_index = v;
		}
	}
	return min_index;
}
	

int receive_data(int port)
{
	int nBytes = recvfrom (sock, &in_data, sizeof(in_data), 0, NULL, NULL);
	printf("received update\n");
	return 0;
}


void send_data()
{
	int sock;
	struct sockaddr_in destAddr[N];
	socklen_t addr_size[N];

	//configure addresses
	int i;
	for (i = 0; i<N; i++)
	{
		destAddr[i].sin_family = AF_INET;
		destAddr[i].sin_port = htons (machines[i].port);
		inet_pton (AF_INET, machines[i].ip, &destAddr[i].sin_addr.s_addr);
		memset (destAddr[i].sin_zero, '\0', sizeof(destAddr[i].sin_zero));
		addr_size[i] = sizeof destAddr[i];

	}

	sock = socket (PF_INET, SOCK_DGRAM, 0);

	for (i=0; i<N; i++)
	{
		if (i != myid)
		{
			sendto(sock, &out_data, sizeof(out_data), 0, (struct sockaddr *)&(destAddr[i]), addr_size[i]);
		}
	}
	return;
}
void print_costs()
{
	int i, j;
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			printf("%d\t",costs[i][j]);
		}
		printf("\n");
	}
	return;
}

	



