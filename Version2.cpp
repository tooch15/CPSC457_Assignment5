
// Version2.cpp

// Favours writers!

#include <pthread.h>
#include <semaphore.h>
#include <cstdio>
#include <cstdlib>

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


	void WriterV2(database* db) {
		//<Entry Section>
		pthread_mutex_lock(&(db->wmutex));
		db->writers++;
		printf("writers is now: %d\n", db->writers);
		if(db->writers == 1){
			printf("In if statement: writers is now: %d\n", db->writers);
			pthread_mutex_lock(&(db->readtry));
			
		}
		printf("Done if statement: writers is now: %d\n", db->writers);
		pthread_mutex_unlock(&(db->wmutex));
		printf("next\n");
		sem_wait(&(db->resource));
		printf("next2\n");
		//<Critical Section>
		printf("writer in CS: writers var is: %d\n", db->writers);
		
		//<Exit Section>
		pthread_mutex_unlock(&(db->readtry));
		pthread_mutex_lock(&(db->wmutex));
		db->writers--;
		if(db->writers == 0){
			pthread_mutex_unlock(&(db->readtry));
		}
		pthread_mutex_unlock(&(db->wmutex));
		sem_post(&(db->resource));
		
	}


	void ReaderV2(database* db){
		//<Entry Section>
		pthread_mutex_lock(&(db->rentry));
		pthread_mutex_lock(&(db->readtry));
		pthread_mutex_lock(&(db->rmutex));
		db->readers++;
		if(db->readers == 1){
			//db->resource
		}
		//<Critical Section>
		
		
		//<Exit Section>
		
		
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
		
		printf("First print in main\n");
		sem_wait(&db.resource);
		
		//Create threads, TODO
		WriterV2(&db);
		
		//Destroy mutexes
		
		int errorCheck2 = pthread_mutex_destroy(&db.wmutex);
		printf("errorCheck %d\n", errorCheck2);
		errorCheck2 += pthread_mutex_destroy(&db.rentry);
		printf("errorCheck %d\n", errorCheck2);
		errorCheck2 += pthread_mutex_destroy(&db.readtry);
		printf("errorCheck %d\n", errorCheck2);
		errorCheck2 += pthread_mutex_destroy(&db.rmutex);
		printf("errorCheck %d\n", errorCheck2);
		if(errorCheck2 != 0){
			printf("We failed to break a mutex with errorCheck %d", errorCheck2);
			exit(1);
		}
		errorCheck2 = sem_destroy(&db.resource);
		if(errorCheck2 == -1){
			printf("We failed to break a semaphore.");
			exit(1);
		}
		
		return 0;
	}


