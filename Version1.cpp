
// Version1.cpp

// Favours readers!

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>  

#define NUM_WRITERS 30
#define NUM_READERS 400

	void* WriterV1(void* arg);
	void* ReaderV1(void* arg);
	
	pthread_mutex_t mutexLock;
	sem_t resourceSem;
	
	struct Database {
		
		int value;
		int readers;
		
	};
		

	void* WriterV1(void* arg) {
		
		
		Database* db = (Database*) arg;
		
		sem_wait(&resourceSem);
		
		printf("Writer locked resource\n");
		
		
		int wait = rand() % 100 + 1;
		wait *= 100000;
		for (int k = 0; k < wait; k++);
		
		
		db->value += 5;
		printf("------------------------Writing %d to the shared variable------------------------\n", db->value);
		
		
		sem_post(&resourceSem);
		
		printf("Writer unlocked resource\n");
		
		pthread_exit(NULL);
		
	}


	void* ReaderV1(void* arg) {
	
		Database* db = (Database*) arg;
	
		pthread_mutex_lock(&mutexLock);
		
		db->readers++;
		
		if (db->readers == 1) {
			sem_wait(&resourceSem);
			printf("Reader locked resource\n");
		}
		
		pthread_mutex_unlock(&mutexLock);
		
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
		
		pthread_exit(NULL);
	
	}


	int main () {
		
		
		Database db = {0, 0};
		db.value = 0;
		db.readers = 0;
		
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
		
		pthread_t readerThread[NUM_READERS];
		pthread_t writerThread[NUM_WRITERS];
		
		int j = 0;
		for (int k = 0; k < NUM_READERS; k++) {
			pthread_create(&readerThread[k], &attr, ReaderV1, arg);
			if (k % 10 == 0 && j < NUM_WRITERS) {
				pthread_create(&writerThread[j], &attr, WriterV1, arg);
				j++;
			}
		}
	
		for (int h = 0; h < NUM_READERS; h++)
			pthread_join(readerThread[h], NULL);
		
		for (int h = 0; h < NUM_WRITERS; h++)
			pthread_join(writerThread[h], NULL);
	
		pthread_exit(NULL);
		
		
		
		return 0;
	
	}

