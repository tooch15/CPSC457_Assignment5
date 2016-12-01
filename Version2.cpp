
// Version2.cpp

// Favours writers!

#include <pthread.h>
#include <semaphore.h>
#include <cstdio>
#include <cstdlib>

	void* WriterV2(void* arg);
	void* ReaderV2(void* arg);
	
#define NUM_WRITERS 10
#define NUM_READERS 100

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
	
} database;


	void* WriterV2(void* input) {
		database* db = (database*) input;
		
		//<Entry Section>
		pthread_mutex_lock(&(db->wmutex));
		db->writers++;
		if(db->writers == 1){
			pthread_mutex_lock(&(db->readtry));
			
		}
		pthread_mutex_unlock(&(db->wmutex));
		
		//Lock resource
		sem_wait(&(db->resource));		
		
		//<Critical Section>
		
		//<Exit Section>
		sem_post(&(db->resource));
		pthread_mutex_lock(&(db->wmutex));
		db->writers--;
		if(db->writers == 0){
			pthread_mutex_unlock(&(db->readtry));
		}
		pthread_mutex_unlock(&(db->wmutex));
		
		pthread_exit(NULL);
		
	}


	void* ReaderV2(void* input){
		database* db = (database*) input;
		
		//<Entry Section>
		pthread_mutex_lock(&(db->rentry));
		pthread_mutex_lock(&(db->readtry));
		pthread_mutex_lock(&(db->rmutex));
		db->readers++;
		if(db->readers == 1){
			sem_wait(&(db->resource));
		}
		pthread_mutex_unlock(&(db->rmutex));
		pthread_mutex_unlock(&(db->readtry));
		pthread_mutex_unlock(&(db->rentry));
		
		//<Critical Section>
		
		
		//<Exit Section>
		pthread_mutex_lock(&(db->rmutex));
		db->readers--;
		if(db->readers == 0){
			sem_post(&(db->resource));
		}
		pthread_mutex_unlock(&(db->rmutex));
		
		pthread_exit(NULL);
		
	}


	int main () {
		//Create database
		database db;
		db.readers = 0;
		db.writers = 0;
		
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
		
		pthread_t readerThread[NUM_READERS];
		pthread_t writerThread[NUM_WRITERS];
		
		int j = 0;
		for (int k = 0; k < NUM_READERS; k++) {
			pthread_create(&readerThread[k], &attr, ReaderV2, arg);
			if (k % 10 == 0) {
				pthread_create(&writerThread[j], &attr, WriterV2, arg);
				j++;
			}
		}
		
		int writer_join_count = 0;
		for(int k = 0; k < NUM_READERS; k++){
			pthread_join(readerThread[k], NULL);
			if (k % 10 == 0) {
				pthread_join(writerThread[writer_join_count], NULL);
				writer_join_count++;
			}
		}
		
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
		
		return 0;
	}


