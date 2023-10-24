#include<iostream>
#include<unistd.h> // fork() + exec() + dup() + sleep()
#include<pthread.h> // pthread
#include<string.h> // string
#include <cmath>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include<fstream>
using std::ofstream;
using namespace std;

int **arr=new int*[256];
int total=0;
sem_t semaphore;

ofstream out; 


int populate(int val)
{   int op=0;
    for(int i=val;i<val+4;i++)
    {
        for(int j=0;j<16;j++)
        {
            arr[i][j]=(rand()%16000)+8000;
            op++;
        }
    }   
    return op;
}

int sum(int val)
{
    int op=0;
    for(int i=val;i<val+4;i++)
    {
        for(int j=0;j<16;j++)
        {
            sem_wait(&semaphore);
            total=total+arr[i][j];
            op++;
            sem_post(&semaphore);
            
        }
    }
    return op;
}

void* thread_fun(void* arg)
{
    int operation_count=0;
    struct timeval start, end;
    
	int val = *(int*)arg;
	
	gettimeofday( &start, NULL );
	operation_count+=populate(val);
	operation_count+=sum(val);
	gettimeofday( &end, NULL );
	
	double seconds = (end.tv_sec - start.tv_sec) +1.0e-6 * (end.tv_usec - start.tv_usec);
    double Gflops = 2e-9*operation_count/seconds;
    sem_wait(&semaphore);
    out<<"cyclic"<<"\t"<<Gflops<<endl;
    sem_post(&semaphore);
	return NULL;
}

int main(int argc,char** argv)
{
	out.open("output_2.txt");
    sem_init(&semaphore,0,1);
    for(int i=0;i<256;i++)
    {
        arr[i]=new int[256];
    }
    
	pthread_t thread[2];

	int position=0;
	int tmp[100];
	for(int j=0;;j++)
	{
    	for(int i=0;i<2;i++)
    	{
    	    tmp[j] = position;
    		pthread_create(&thread[i],NULL,thread_fun,(void*)&tmp[i]);
    		position=position+4;
    	}
		if(position>=16)
		{
		    break;
		}
	}
	for(int i=0;i<2;i++)
	{
		pthread_join(thread[i],NULL);
	}
    cout<<total;
    out.close();
    return 0;
}
