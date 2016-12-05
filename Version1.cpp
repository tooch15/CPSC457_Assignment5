
// Version1.cpp

// Favours readers!

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>  
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

#define NUM_FIXED_THREADS 100

void* WriterV1(void* arg);
void* ReaderV1(void* arg);

pthread_mutex_t mutexLock;
sem_t resourceSem;

volatile bool wait = true;

struct Database {
	
	int value;
	int readers;
	
};

using namespace std::chrono;

double totalTurnAroundTime_Readers;
double totalTurnAroundTime_Writers;

void* WriterV1(void* arg) {
	
	Database* db = (Database*) arg;
	
	high_resolution_clock::time_point begin = high_resolution_clock::now();
	
	sem_wait(&resourceSem);
	
	printf("Writer locked resource\n");
	
	
	int wait = rand() % 100 + 1;
	wait *= 100000;
	for (int k = 0; k < wait; k++);
	
	db->value += 5;
	printf("------------------------Writing %d to the shared variable------------------------\n", db->value);
	
	sem_post(&resourceSem);
	
	printf("Writer unlocked resource\n");
	
	high_resolution_clock::time_point end = high_resolution_clock::now();
	
	duration<double> time_span = duration_cast<duration<double>>(end - begin);
	
	totalTurnAroundTime_Writers += time_span.count();
	
	pthread_exit(NULL);
	
}


void* ReaderV1(void* arg) {

	Database* db = (Database*) arg;

	high_resolution_clock::time_point begin = high_resolution_clock::now();

	pthread_mutex_lock(&mutexLock);
	
	db->readers++;
	
	if (db->readers == 1) {
		sem_wait(&resourceSem);
		printf("Reader locked resource\n");
	}
	
	pthread_mutex_unlock(&mutexLock);
	
	// simulate operations that the reader may need to perform in its critical section
	int wait = rand() % 100 + 1;
	wait *= 100000;
	for (int k = 0; k < wait; k++);

	printf("The value of the resource in the database is: %d\n", db->value);
	
	pthread_mutex_lock(&mutexLock);
	
	db->readers--;
	
	if (db->readers == 0) {
		sem_post(&resourceSem);
		printf("Reader unlocked resource\n");
	}
	
	pthread_mutex_unlock(&mutexLock);
	
	high_resolution_clock::time_point end = high_resolution_clock::now();
	
	duration<double> time_span = duration_cast<duration<double>>(end - begin);
	
	totalTurnAroundTime_Readers += time_span.count();
	
	pthread_exit(NULL);

}


void server () {
		
int num_readers = 10;
int num_writers = 10;

//for (int num_writers = 5; num_writers < 505; num_writers += 5) { 

	Database db = {0, 0};
	db.value = 0;
	db.readers = 0;
	
	totalTurnAroundTime_Readers = totalTurnAroundTime_Writers = 0;
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	int semFailCheck = sem_init(&resourceSem, 0, 1);
	
	if (semFailCheck == -1)
		printf("Error creating semaphore");
			
	int errorCheck = pthread_mutex_init(&mutexLock, NULL);
	
	if (errorCheck != 0) {
		printf("The initilization of the mutex lock encountered an error with code: %d", errorCheck);
	}
	
	void *arg = (void*)(&db);
	
	pthread_t readerThread[num_readers];
	pthread_t writerThread[num_writers];
	
	int k = 0;
	while (true) {
		
		if (k < num_readers)
			pthread_create(&readerThread[k], &attr, ReaderV1, arg);
		
		if (k < num_writers)
			pthread_create(&writerThread[k], &attr, WriterV1, arg);
			
		if ( k >= num_writers && k >= num_readers)
			break;
		
		k++;
		
	}

	for (int h = 0; h < num_readers; h++)
		pthread_join(readerThread[h], NULL);
	
	for (int h = 0; h < num_writers; h++)
		pthread_join(writerThread[h], NULL);
	
	//std::cout << "Total turnaround time for all Readers: " << totalTurnAroundTime_Readers << std::endl;
	//std::cout << "Total turnaround time for all Writers: " << totalTurnAroundTime_Writers << std::endl;
	
	double avgTurnAroundTime_Readers = totalTurnAroundTime_Readers / num_readers;
	double avgTurnAroundTime_Writers = totalTurnAroundTime_Writers / num_writers;
	
	//std::cout << num_readers << ": " << avgTurnaroundTime_Readers << std::endl;
	//std::cout << num_writers << ": " << avgTurnaroundTime_Writers << std::endl;
	//std::cout << avgTurnAroundTime_Readers << std::endl;
	//std::cout << avgTurnAroundTime_Writers << std::endl;

//}

	pthread_exit(NULL);

}

int main() {
	server();
	return 0;
}

