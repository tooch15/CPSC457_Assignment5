
// Version2.cpp

// Favours writers!

#include <pthread.h>
#include <semaphores.h>
#include <cstdio>

typedef struct {
	int writers = 0;
	int readers = 0;
	
	//Semaphores - wmutex, rmutex and rentry are mutexes. 
	//readtry and resource are semaphores
	pthread_mutex_t wmutex; //Used by writers
	pthread_mutex_t rentry; //Used by readers
	pthread_mutex_t readtry; //Used by readers/writers
	pthread_mutex_t rmutex; //Used by readers
	sem_t resource;
	
} database;  

	
	
	//Global writers resource
	int writers = 0;


	void WriterV2(struct database* db) {
		//<Entry Section>
		pthread_mutex_lock(&db.wmutex);
		writers++;
		if(writers == 1){
			pthread_mutex_lock(&db.readtry);
		}
		pthread_mutex_unlock(&db.wmutex);
		
		
		//<Critical Section>
		
		
		//<Exit Section>
		
	}


	void ReaderV2() {
		
		
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
		
		errorCheck =  sem_init(&db.resource,0,0);
		if(errorCheck == -1){
			printf("A semaphore initialization failed");
			exit(1);
		}
		
		//Create threads, TODO
		
		
		//Destroy mutexes
		
		int errorCheck2 = pthread_mutex_destroy(&db.wmutex);
		errorCheck2 += pthread_mutex_destroy(&db.rentry);
		errorCheck2 += pthread_mutex_destroy(&db.readytry);
		errorCheck2 += pthread_mutex_destroy(&db.rmutex);
		if(errorCheck2 != 0){
			printf("We failed to break a mutex.");
			exit(1);
		}
		errorCheck2 = sem_destroy(&db.resource);
		if(errorCheck2 == -1){
			printf("We failed to break a semaphore.");
			exit(1);
		}
		
		return 0;
	}


