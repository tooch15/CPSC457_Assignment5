
// Version3.cpp

// This version is fair!

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>  
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>


void* WriterV3(void* arg);
void* ReaderV3(void* arg);

sem_t resourceSem;
sem_t mutexSem;

struct Database {
	
	int value;
	int readers;
	int writers;
	
};
	
double totalTurnAroundTime_Readers;
double totalTurnAroundTime_Writers;	

using namespace std::chrono;

void* WriterV3(void* arg) {
	
	
	Database* db = (Database*) arg;
	
	high_resolution_clock::time_point begin = high_resolution_clock::now();		
	
	sem_wait(&mutexSem);
	
	db->writers++;
	
	sem_post(&mutexSem);
	
	sem_wait(&resourceSem);
	
	printf("Writer locked resource\n");
	
	
	int wait = rand() % 100 + 1;
	wait *= 100000;
	for (int k = 0; k < wait; k++);
	
	
	db->value += 5;
	printf("------------------------Writing %d to the shared variable------------------------\n", db->value);
	
	sem_wait(&mutexSem);
	
	db->writers++;
	
	sem_post(&mutexSem);
	
	sem_post(&resourceSem);
	
	printf("Writer unlocked resource\n");
	
	high_resolution_clock::time_point end = high_resolution_clock::now();
	
	duration<double> time_span = duration_cast<duration<double>>(end - begin);
	
	totalTurnAroundTime_Writers += time_span.count();
	
	pthread_exit(NULL);
	
}


void* ReaderV3(void* arg) {

	Database* db = (Database*) arg;
	
	high_resolution_clock::time_point begin = high_resolution_clock::now();

	sem_wait(&mutexSem);
	
	if (db->writers > 0 || db->readers == 0) {
		sem_post(&mutexSem);
		sem_wait(&resourceSem);
		printf("Reader locked resource\n");
		sem_wait(&mutexSem);			
	}
	
	db->readers++;
	
	sem_post(&mutexSem);
	
	int wait = rand() % 100 + 1;
	wait *= 100000;
	for (int k = 0; k < wait; k++);
	
	printf("The value of the resource in the database is: %d\n", db->value);
	
	
	db->readers--;
	
	sem_wait(&mutexSem);
	
	if (db->readers == 0) {
		sem_post(&resourceSem);
		printf("Reader unlocked resource\n");
	}
	
	
	sem_post(&mutexSem);
	
high_resolution_clock::time_point end = high_resolution_clock::now();
	
	duration<double> time_span = duration_cast<duration<double>>(end - begin);
	
	totalTurnAroundTime_Readers += time_span.count();
	
	pthread_exit(NULL);

}


void server() {
	
	
	Database db = {0, 0, 0};
	
	int num_writers = 10;
	int num_readers = 10;

	//for (int num_writers = 5; num_writers < 505; num_writers += 5) { 	
		
		totalTurnAroundTime_Readers = totalTurnAroundTime_Writers = 0;
		
		db.value = 0;
		db.readers = 0;
		db.writers = 0;
		
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		
		int semFailCheck = sem_init(&resourceSem, 0, 1);
		
		if (semFailCheck == -1)
			printf("Error creating semaphore");
		
		semFailCheck = sem_init(&mutexSem, 0, 1);
		
		if (semFailCheck == -1)
			printf("Error creating semaphore");
				
		
		void *arg = (void*)(&db);
		
		pthread_t readerThread[num_readers];
		pthread_t writerThread[num_writers];
		
		int k = 0;
		while (true) {
			
			if (k < num_writers)
				pthread_create(&writerThread[k], &attr, WriterV3, arg);
			
			if (k < num_readers)
				pthread_create(&readerThread[k], &attr, ReaderV3, arg);
				
			if ( k >= num_writers && k >= num_readers)
				break;
			
			k++;
			
		}
	
		for (int h = 0; h < num_readers; h++)
			pthread_join(readerThread[h], NULL);
		
		for (int h = 0; h < num_writers; h++)
			pthread_join(writerThread[h], NULL);
		
		//std::cout << "Total turnaround time for all Writers: " << totalTurnAroundTime_Writers << std::endl;
		//std::cout << "Total turnaround time for all Readers: " << totalTurnAroundTime_Readers << std::endl;
		
		double avgTurnaroundTime_Writers = totalTurnAroundTime_Writers / num_writers;
		double avgTurnaroundTime_Readers = totalTurnAroundTime_Readers / num_readers;
		
		//std::cout << num_writers << ": " << avgTurnaroundTime_Writers << std::endl;
		//std::cout << avgTurnaroundTime_Writers << std::endl;
		
		//std::cout << num_readers << ": " << avgTurnaroundTime_Readers << std::endl;
		//std::cout << avgTurnaroundTime_Readers << std::endl;
			
	//}
	
		pthread_exit(NULL);

}

int main() {
	server();
	return 0;
}

