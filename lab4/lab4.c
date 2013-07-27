//
//  lab4.c
//  CSE2431
//
//  Created by iqbal hasnan.1@osu.edu and Dylan Frost.149@osu.edu on 7/20/13.
//
//  Compile
//  =========
//  gcc -O1 -Wall -o lab4 lab4.c -lpthread -std=gnu99
//
//  Run
//  =========
//  ./lab4 ThreadNumber
//
//  Terminate
//  =========
//  ctrl + c
//
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define n 1200
#define m 1000
#define p 500
#define MAX_THREAD 7 //BONUS POINT

/* Global Variables */
int a[n][m], b[m][p], c[n][p], c1[n][p]; //matrix (nxm) x (mxp) = (nxp)
int status;
int threadNumber;

/* function prototype */
void *multiplication(void *param); /* the multiplication thread */
void init_a(int a[n][m]);
void init_b(int b[m][p]);
void display_matrix(int c[n][p]);
void matrix_c1();

int main(int argc, char *argv[]){
    
    pthread_t tid_matrix[MAX_THREAD];
    pthread_attr_t attr_matrix; //thread attributes for matrix
    struct timeval start, end;      // time variables
    
    if(argc != 2) {
        printf("Error: provide exactly 1 arguments \n");
        printf("Usage: ./lab4 int \n"); //check the number of arguments passed to the main
        exit(1); //terminate the program
    }
    
    int userInput = atoi(argv[1]); //import from user

    threadNumber = 2; //start running with 2 threads
    
    pthread_attr_init(&attr_matrix); //get the default attributes for matrix
    
    init_a(a); //initialize matrix a
    init_b(b); //initialize matrix b
    
    gettimeofday(&start, NULL); //start time for c1
    matrix_c1();                //running the c1 multiplication
    gettimeofday(&end, NULL);   //end time for c1
    printf("C1 Time : %f seconds\n",((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec))/1000000.0);
    
    //display_matrix(c1); //display 20 elements of matrix c1

    while(threadNumber <= userInput){ //displaying 7 threads for Bonus Point !
        gettimeofday(&start, NULL);
        /* 3. Create multiplication thread(s) */
        for(long i = 0; i < threadNumber; i++) {
            //create i number of multiplication thread(s) based on the argument passed
            status = pthread_create(&tid_matrix[i],&attr_matrix,multiplication,(void*)i);
            if(status!=0){
                printf("Error creating thread");
                exit(-1);
            }
        }
        /* 4. Release resources */
        for(int j = 0; j < threadNumber; j++) {
            pthread_join(tid_matrix[j],NULL); //waiting for other thread to complete
        }
        gettimeofday(&end, NULL);
        printf("\n\nThread(s) %d, Seconds: %f",threadNumber,((end.tv_sec * 1000000 + end.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec))/1000000.0);
        
        if(status == 0){
            printf("\nNo Error");
        }
        threadNumber++;
    }
    
    //display_matrix(c); //display metrix element of c
    
    exit(0); //exit program
}

/* initialize matrix A */
void init_a(int a[n][m]){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            a[i][j] = i + 1;
        }
    }
}

/* initialize matrix B */
void init_b(int b[m][p]){
    for(int i = 0; i < m; i++){
        for(int j = 0; j < p; j++){
            b[i][j] = i + j;
        }
    }
}


/* print matrix C for value checking 20 - 30 elements */
void display_matrix(int c[n][p]){
    printf("\n20 elements of A x B = C \n");
    for (int i = 0; i < 1 ; i++) {
        printf("\n| ");
        for (int j = 0; j < 20 ; j++)
            printf("%2d ", c[i][j]);
        printf("|");
    }
}

/* Multiplication Matrix A x B */
void *multiplication(void *param){
    
    long thread = (long)param; //thread id info
    long rowStart = (thread* n)/threadNumber; //divide the thread with number of row each thread start processing
    long rowEnd = ((thread+1)* n)/threadNumber; //number of row each thread stop processing
    
    /* algorithm for multiplication A x B = C */
    for(long i = rowStart; i < rowEnd; i++){
        for(int j =0; j < p; j++){
            c[i][j]=0;
            for(int k = 0; k < m; k++){
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return 0; //return 0 to pthread_create when successfull
}

/*  matrix C1 */
void matrix_c1(){
    for(int i = 0; i < n; i++){
        for(int j =0; j < p; j++){
            c1[i][j]=0;
            for(int k = 0; k < m; k++){
                c1[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    
}
