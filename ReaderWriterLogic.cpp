#include <pthread.h>


pthread_mutex_t mutex;



void WriterV1() {
	
	pthread_mutex_lock(&mutex);
	
}

void WriterV2() {
	
}

void WriterV3() {
	
}



void ReaderV1() {
	
}

void ReaderV2() {
	
}

void ReaderV3() {
	
}
