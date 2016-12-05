
// Version2.cpp

// Favours writers!

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>  
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

void* WriterV2(void* arg);
void* ReaderV2(void* arg);

double totalTurnAroundTime_Readers;
double totalTurnAroundTime_Writers;

using namespace std::chrono;

typedef struct {
	int writers;
	int readers;
	
	//Semaphores - wmutex, rmutex and rentry are mutexes. 
	//readtry and resource are semaphores
	pthread_mutex_t wmutex; //Used by writers
	pthread_mutex_t rentry; //Used by readers
	pthread_mutex_t rmutex; //Used by readers
	
	pthread_mutex_t readtry; //Used by readers/writers
	sem_t resource;
	
	int value;
	
} database;


void* WriterV2(void* input) {
	
	high_resolution_clock::time_point begin = high_resolution_clock::now();
	
	database* db = (database*) input;
	
	//<Entry Section>
	pthread_mutex_lock(&(db->wmutex));
	db->writers++;
	if(db->writers == 1){
		pthread_mutex_lock(&(db->readtry));
		printf("Writer locked readtry\n");
	}
	pthread_mutex_unlock(&(db->wmutex));
	
	//Lock resource
	sem_wait(&(db->resource));	

	//<Critical Section>
	int wait = rand() % 100 + 1;
	wait *= 100000;
	for (int k = 0; k < wait; k++);
	
	
	db->value += 5;
	printf("------------------------Writing %d to the shared variable------------------------\n", db->value);
	
	
	//<Exit Section>
	sem_post(&(db->resource));

	pthread_mutex_lock(&(db->wmutex));
	db->writers--;
	if(db->writers == 0){
		pthread_mutex_unlock(&(db->readtry));
		printf("Writer unlocked readtry\n");
	}
	pthread_mutex_unlock(&(db->wmutex));
	
	high_resolution_clock::time_point end = high_resolution_clock::now();
	
	duration<double> time_span = duration_cast<duration<double>>(end - begin);
	
	totalTurnAroundTime_Writers += time_span.count();
	
	pthread_exit(NULL);
	
}


void* ReaderV2(void* input){
	database* db = (database*) input;
	
	high_resolution_clock::time_point begin = high_resolution_clock::now();
	
	//<Entry Section>
	pthread_mutex_lock(&(db->rentry));
	pthread_mutex_lock(&(db->readtry));
	pthread_mutex_lock(&(db->rmutex));
	db->readers++;
	if(db->readers == 1){
		sem_wait(&(db->resource));
		printf("Reader locked resource\n");
	}
	pthread_mutex_unlock(&(db->rmutex));
	pthread_mutex_unlock(&(db->readtry));
	pthread_mutex_unlock(&(db->rentry));
	
	//<Critical Section>
	int wait = rand() % 100 + 1;
	wait *= 100000;
	for (int k = 0; k < wait; k++);
	
	printf("The value of the resource in the database is: %d\n", db->value);
	
	//<Exit Section>
	pthread_mutex_lock(&(db->rmutex));
	db->readers--;
	if(db->readers == 0){
		sem_post(&(db->resource));
		printf("Reader unlocked resource\n");
	}
	pthread_mutex_unlock(&(db->rmutex));
	
	high_resolution_clock::time_point end = high_resolution_clock::now();
	
	duration<double> time_span = duration_cast<duration<double>>(end - begin);
	
	totalTurnAroundTime_Readers += time_span.count();
	
	pthread_exit(NULL);
	
}


void server () {
	
	database db;
	
	int num_readers = 10;
	int num_writers = 10;
	
//for (int num_writers = 5; num_writers < 505; num_writers += 5) { 

	totalTurnAroundTime_Readers = totalTurnAroundTime_Writers = 0;
	
	//Create database
	db.readers = 0;
	db.writers = 0;
	db.value = 0;
	
	//Initialize mutexes
	int errorCheck = pthread_mutex_init(&db.wmutex, NULL);
	errorCheck += pthread_mutex_init(&db.rentry, NULL);
	errorCheck += pthread_mutex_init(&db.readtry, NULL);
	errorCheck += pthread_mutex_init(&db.rmutex, NULL);
	if (errorCheck != 0){
		printf("A mutex initialization failed");
		exit(1);
	}
	
	errorCheck =  sem_init(&db.resource,0,1);
	if(errorCheck == -1){
		printf("A semaphore initialization failed");
		exit(1);
	}
	
	//Create Threads
	
	void *arg = (void*)(&db);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	pthread_t readerThread[num_readers];
	pthread_t writerThread[num_writers];

	int k = 0;
	while (true) {
		
		if (k < num_writers)
			pthread_create(&writerThread[k], &attr, WriterV2, arg);
		
		if (k < num_readers)
			pthread_create(&readerThread[k], &attr, ReaderV2, arg);
			
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
	
	
	//Destroy mutexes
	
	int errorCheck2 = pthread_mutex_destroy(&db.wmutex);
	errorCheck2 += pthread_mutex_destroy(&db.rentry);
	errorCheck2 += pthread_mutex_destroy(&db.readtry);
	errorCheck2 += pthread_mutex_destroy(&db.rmutex);
	if(errorCheck2 != 0){
		printf("We failed to break down a mutex with errorCheck %d", errorCheck2);
		exit(1);
	}
	errorCheck2 = sem_destroy(&db.resource);
	if(errorCheck2 == -1){
		printf("We failed to break a semaphore.");
		exit(1);
	}

}

int main() {
	server();
}


