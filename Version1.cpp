
// Version1.cpp

// Favours readers!

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

	void* WriterV1(void* arg);
	void* ReaderV1(void* arg);
	void setLock(int lock);
	void resetLock(int lock);
	
	pthread_mutex_t mutexLock;
	int resourceLock;
	
	sem_t resourceSem;
	
	struct Database {
		
		int value;
		int readers;
		
	};
		

	void* WriterV1(void* arg) {
		
		printf("Inside of Writer\n");
		
		Database* db = (Database*) arg;
		
		//setLock(resourceLock);
		sem_wait(&resourceSem);
		
		printf("Writer locked resource\n");
		
		
		db->value += 5;
		
		
		//resetLock(resourceLock);
		sem_post(&resourceSem);
		
		printf("Writer unlocked resource\n");
		
		pthread_exit(NULL);
		
	}


	void* ReaderV1(void* arg) {
		
		printf("Inside of Reader\n");
	
		Database* db = (Database*) arg;
	
		pthread_mutex_lock(&mutexLock);
		
		db->readers++;
		
		if (db->readers == 1) {
			sem_wait(&resourceSem);
			//setLock(resourceLock);
		}
		
		printf("Reader locked resource\n");
		
		pthread_mutex_unlock(&mutexLock);
		
		printf("The value of the resource in the database is: %d\n", db->value);
		
		pthread_mutex_lock(&mutexLock);
		
		db->readers--;
		
		if (db->readers == 0) {
			sem_post(&resourceSem);
			//resetLock(resourceLock);
		}
		
		printf("Reader unlocked resource\n");
		
		
		pthread_mutex_unlock(&mutexLock);
		
		pthread_exit(NULL);
	
	}


	int main () {
		
		
		Database db = {0, 0};
		db.value = 0;
		db.readers = 0;
		
		resourceLock = 0;
		
		sem_init(&resourceSem, 0, 1);
				
		int errorCheck = pthread_mutex_init(&mutexLock, NULL);
		
		if (errorCheck != 0) {
			printf("The initilization of the mutex lock encountered an error with code: %d", errorCheck);
		//	exit(1);
		}
		
		void *arg = (void*)(&db);
		
		pthread_t readerThread;
		pthread_t writerThread;
		

		pthread_create(&readerThread, NULL, ReaderV1, arg);
		pthread_create(&writerThread, NULL, WriterV1, arg);

		pthread_exit(NULL);
		
		
		
		return 0;
	
	}
	
	void setLock(int lock) {
		
		while (1) {
			
			int check = __sync_val_compare_and_swap(&lock, 0, 1);
			
			if (check == 0)
				break;
			
		}
		
	}
	
	void resetLock(int lock) {
		
		__sync_val_compare_and_swap(&lock, 1, 0);
		
	}

