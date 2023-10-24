#include<iostream>
#include<unistd.h> // fork() + exec() + dup() + sleep()
#include<pthread.h> // pthread
#include<string.h> // string
#include <cmath>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <semaphore.h>
using namespace std;

int s=pow(2,16);
int* arr;
int k=pow(2,16)/4;
float sum=0.0;
float mean=0.0;
float variance=0.0;
sem_t semaphore;
int value[4];
int indx=0;

int populate(int val)
{
	int op=0;
    for(int i=val-k;i<val;i++)
    {
        arr[i]=rand()%100;
        op++;
    }
    return op;
}

int calcSqrt(int val)
{
	int op=0;
    for(int i=val-k;i<val;i++)
    {
        arr[i]=sqrt(arr[i]);
        op++;
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
	operation_count+=calcSqrt(val);
	gettimeofday( &end, NULL );
	
	double seconds = (end.tv_sec - start.tv_sec) +1.0e-6 * (end.tv_usec - start.tv_usec);
    	double Gflops = 2e-9*operation_count/seconds;
    	printf( " Performance in Gflops %.3f Gflop/s\n", Gflops );
    	sem_wait(&semaphore);
    	value[indx]=Gflops;
    	indx++;
   	sum+=Gflops;
    	sem_post(&semaphore);
	return NULL;
}

int main(int argc,char** argv)
{
	sem_init(&semaphore,0,1);
	arr=new int[s];
	int cores = 0;
	string a = "cpu";	
	string b = "cores";	
	for(int i=0;i<argc;i++)
	{
		string c = argv[i];
		string d = argv[i+1];
		if(a==c && b==d)
		{
			cores = atoi(argv[i+3]);
			break;
		}
	}
	
	cpu_set_t cpu[cores];
	pthread_t thread[cores];
	
	for(int i=0;i<cores;i++)
	{
		CPU_ZERO(&cpu[i]);
		CPU_SET(i,&cpu[i]);
	}
	cout<<"With Affinity: "<<endl;
	int position=pow(2,16)/4;
	int tmp[cores];
	for(int i=0;i<cores;i++)
	{
	    tmp[i] = position;
		pthread_create(&thread[i],NULL,thread_fun,(void*)&tmp[i]);
		position=position+k;
	}
	for(int i=0;i<cores;i++)
	{
		pthread_setaffinity_np(thread[i],sizeof(cpu_set_t),&cpu[i]);
	}

	for(int i=0;i<cores;i++)
	{
		pthread_join(thread[i],NULL);
	}
	mean=sum/4;
	for(int i=0;i<4;++i)
	{
		variance+=pow(value[i]-mean,2);
	}
	variance/=4;
	cout<<"Variance for with affinity: "<<variance<<endl<<endl;
	cout<<endl;
	
	sum=0;
	indx=0;
	mean=0;
	variance=0;
	
	cout<<"Without Affinity: "<<endl;
	position=pow(2,16)/4;
	for(int i=0;i<cores;i++)
	{
	    tmp[i] = position;
		pthread_create(&thread[i],NULL,thread_fun,(void*)&tmp[i]);
		position=position+k;
	}
	
	for(int i=0;i<cores;i++)
	{
		pthread_join(thread[i],NULL);
	}
	mean=sum/4;
	for(int i=0;i<4;++i)
	{
		variance+=pow(value[i]-mean,2);
	}
	variance/=4;
	cout<<"Variance for without affinity: "<<variance<<endl<<endl;
	cout<<endl;
	cout<<"Whole Array: "<<endl;
	for(int i=0;i<pow(2,16);i++)
		cout<<arr[i]<<" ";

  
    return 0;
}
