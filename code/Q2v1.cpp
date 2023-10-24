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

int **arr=new int*[16];
int total=0;
sem_t semaphore;
ofstream out;
string name;

struct pos
{
    int s=0;
    int row=0;
    int start=0;
    int end=0;
};

int populate(int row,int start,int end,int s)
{   
    int op=0;
    for(int i=row;i<s;i++)
    {
        for(int j=start;j<=end;j++)
        {
            arr[i][j]=(rand()%16000)+8000;
            op++;
        }
    }   
    return op;
}

int sum(int row,int start,int end,int s)
{
    int op=0;
    for(int i=row;i<s;i++)
    {
        for(int j=start;j<=end;j++)
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
	pos* p = (pos*)arg;
	
	gettimeofday( &start, NULL );
	operation_count+=populate(p->row,p->start,p->end,p->s);
	operation_count+=sum(p->row,p->start,p->end,p->s);
	gettimeofday( &end, NULL );
	
	double seconds = (end.tv_sec - start.tv_sec) +1.0e-6 * (end.tv_usec - start.tv_usec);
    double Gflops = 2e-9*operation_count/seconds;
    sem_wait(&semaphore);
    out<<name<<"\t"<<Gflops<<endl;
    sem_post(&semaphore);
	return NULL;
}

int main(int argc,char** argv)
{
    out.open("output.txt");
    sem_init(&semaphore,0,1);
    
    for(int i=0;i<16;i++)
    {
        arr[i]=new int[16];
    }
    
    int t = 16*16;
    
    int size_4 = t/4;
	pthread_t thread_1[size_4];

	int size_16 = t/16;
	pthread_t thread_2[size_16];
	
	int size_256 = t/256;
	pthread_t thread_3[size_256];

// FOR 2^2
    name="2^2";
	pos position_size_4[16][4];
	int start=0;
	int end=3;
	for(int i=0;i<16;i++)
	{
	    for(int j=0;j<4;j++)
	    {
	        position_size_4[i][j].s=i+1;
	        position_size_4[i][j].row=i;
	        position_size_4[i][j].start=start;
	        position_size_4[i][j].end=end;
	        start+=4;
	        end+=4;
	        
	       // cout<<"row: "<<position_size_4[i][j].row<<", start: "<<position_size_4[i][j].start<<", end:"<<position_size_4[i][j].end<<endl;
	    }
	    start=0;
	    end=3;
	}
	int th=0;
	for(int i=0;i<16;i++)
	{
	    for(int j=0;j<4;j++)
	    {
	        pthread_create(&thread_1[th],NULL,&thread_fun,(void*)&position_size_4[i][j]);
	        th++;
	    }
	}
	for(int i=0;i<64;i++)
	{
		pthread_join(thread_1[i],NULL);
	}
	
	
// FOR 2^4
    name="2^4";
	pos position_size_16[16];
	for(int i=0;i<16;i++)
	{
        position_size_16[i].s=i+1;
        position_size_16[i].row=i;
        position_size_16[i].start=0;
        position_size_16[i].end=15;
        // cout<<"row: "<<position_size_16[i].row<<", start: "<<position_size_16[i].start<<", end:"<<position_size_16[i].end<<endl;
	}
    for(int j=0;j<16;j++)
    {
        pthread_create(&thread_2[j],NULL,&thread_fun,(void*)&position_size_16[j]);
    }
	for(int i=0;i<16;i++)
	{
		pthread_join(thread_2[i],NULL);
	}
	
	
	// FOR 2^8
	 name="2^8";
	pos position_size_64;

    position_size_64.s=16;
    position_size_64.row=0;
    position_size_64.start=0;
    position_size_64.end=15;
    // cout<<"row: "<<position_size_64.row<<", start: "<<position_size_64.start<<", end:"<<position_size_64.end<<endl;


    pthread_create(&thread_3[0],NULL,&thread_fun,(void*)&position_size_64);
    
	pthread_join(thread_3[0],NULL);
	out.close();
    return 0;
}
