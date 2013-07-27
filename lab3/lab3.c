//
//  lab3.c
//  CSE2431
//
//  Created by iqbal hasnan and Dylan Frost on 7/4/13.
//
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

typedef int buffer_item;
#define BUFFER_SIZE 8

void *producer(void *param); /* the producer thread */
void *consumer(void *param); /* the consumer thread */

int insert_item(buffer_item item);
int remove_item(buffer_item *item);

//global variables
pthread_mutex_t mutex;            //the mutex lock attribute
sem_t full, empty;                //semaphore full and empty attribute
buffer_item buffer[BUFFER_SIZE];  //the buffer
int counter;                      //buffer counter
unsigned int seed;                  //rand_r() seed


/* Producer Thread */
void *producer(void *param) {
    buffer_item rand;
    
    while(1) {
        /* sleep for a random period of time */
        int RandomSleep = rand_r(&seed)%50+1; //random number generated between 1 to 50
        sleep(RandomSleep);
        
        /* generate a random number */
        rand = rand_r(&seed);
        
        //acquire the empty lock
        sem_wait(&empty);
        //acquire the mutex lock
        pthread_mutex_lock(&mutex);
        
        //critical section
        if(insert_item(rand) < 0) {
            printf(" Producer Error : Buffer is Full!\n");
        }
        
        //release the mutex lock
        pthread_mutex_unlock(&mutex);
        //signal full
        sem_post(&full);
    }
}

/* Consumer Thread */
void *consumer(void *param) {
    buffer_item rand;
    
    while(1) {
        /* sleep for a random period of time */
        int RandomSleep = rand_r(&seed)%50+1; //random number generated between 1 to 50
        sleep(RandomSleep);
        
        //aquire the full lock
        sem_wait(&full);
        //aquire the mutex lock
        pthread_mutex_lock(&mutex);
        
        //critical section
        if(remove_item(&rand) < 0 ) {
            printf("Consumer Error : Buffer is empty !\n");
        }
        
        //release the mutex lock
        pthread_mutex_unlock(&mutex);
        //signal empty
        sem_post(&empty);
    }
}


int insert_item(buffer_item item) {
    //when the buffer is not full, add the item intoo the buffer, and increment the counter
    if(counter < BUFFER_SIZE) {
        buffer[counter++] = item;
        printf("producer produced %d\n", item);
        return 0;
    }
    else {
        return -1; //buffer is full
    }
}


int remove_item(buffer_item *item) {
    //when the buffer is not empty, remove the item from the buffer, and decrement the counter
    if(counter > 0) {
        *item = buffer[--counter];
        printf("consumer consumed %d\n", *item);
        return 0;
    }
    else {
        return -1; //buffer is empty
    }
}

int main(int argc, char *argv[]) {
    pthread_t tid_producer[BUFFER_SIZE], tid_consumer[BUFFER_SIZE]; //thread ID for producer and consumer
    pthread_attr_t attr_producer, attr_consumer; //thread attributes for producer and consumer
    if(argc != 4) {
        printf("Error: provide exactly 3 arguments \n");
        printf("Usage: ./lab3 int int int \n"); //check the number of arguments passed to the main
        exit(1); //terminate the program
    }
    
    /* 1. Get command line arguments argv[1], argv[2], argv[3] */
    int SleepTime = atoi(argv[1]);      //How long to sleep before terminating.
    int ProducerThreadNumber = atoi(argv[2]); //The number of producer threads.
    int ConsumerThreadNumber = atoi(argv[3]); //The number of consumer threads.
    
    /* 2. Initialize buffer, mutex, semaphores, and other global vars */
    pthread_mutex_init(&mutex, NULL); //create the mutex lock
    sem_init(&full, 0, 0);            //create the full semaphore and initialize to 0
    sem_init(&empty, 0, BUFFER_SIZE); //create the empty semaphore and initialize to BUFFER_SIZE
    pthread_attr_init(&attr_producer); //get the default attributes for producer
    pthread_attr_init(&attr_consumer); //get the default attributes for consumer
    counter = 0; //buffer counter initialize to 0
    
    
    /* 3. Create producer thread(s) */
    for(int i = 0; i < ProducerThreadNumber; i++) {
        pthread_create(&tid_producer[i],&attr_producer,producer,NULL);//create i number of producer thread based on the argument passed
    }
    
    /* 4. Create consumer thread(s) */
    for(int j = 0; j < ConsumerThreadNumber; j++) {
        pthread_create(&tid_consumer[j],&attr_consumer,consumer,NULL);//create j number of consumer thread based on the argument passed
    }
    
    /* 5. Sleep */
    sleep(SleepTime);
    
    /* 6. Release resources, e.g. destroy mutex and semaphores */
    for(int i = 0; i < ProducerThreadNumber;i++){
        pthread_join(tid_producer[i],NULL); //calling producer threads wait until the given thread terminates
    }
    
    for(int j = 0; j < ConsumerThreadNumber; j++){
        pthread_join(tid_consumer[j],NULL); //calling consumer threads wait untill the given thread terminates
    }
    
    /* 7. Exit */
    exit(0); //exit
    
}

