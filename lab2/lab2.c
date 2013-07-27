//
//  shell.c
//  CSE2431
//
//  Created by iqbal hasnan on 6/17/13.
//  Copyright (c) 2013 iqbal hasnan. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define HISTORY_SIZE 5 /*keep track of 5 most recent commands*/

int cmd_count; /*global to keep track of most recent commands entered*/
char history[HISTORY_SIZE][MAX_LINE]; /* global so it can be accessed in interrupt handler. */

/**
 * print command
 */
void viewHistory()
{
    int i;
    
    if (cmd_count < 1)
        printf("No command history to show. \n");
    else {
        printf("\n\n");
        for (i = (cmd_count >= HISTORY_SIZE) ? cmd_count - HISTORY_SIZE:0;
             i < cmd_count; i++)
            printf("%d: %s\n",i+1,history[i%HISTORY_SIZE]);
    }
    //printf("SystemsIIShell->");
}

/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a
 * null-terminated string.
 */

int setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
    i,      /* loop index for accessing inputBuffer array */
    start,  /* index where beginning of next command parameter is */
    ct;     /* index of where to place the next parameter into args[] */
    
    int temp;
    
    ct = 0;
    
    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
    
    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0){
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }else{
        inputBuffer[length]='\0';
        if(inputBuffer[0]=='r'){
            if(inputBuffer[1]=='r'){
                if(cmd_count==0){
                    printf("No recent command can be found in the history. \n");
                    return 0;
                }
                strcpy(inputBuffer,history[(cmd_count)% HISTORY_SIZE]);
            }else{
                temp = atoi(&inputBuffer[1]);
                if(temp < 1 || temp > cmd_count || temp <= cmd_count -HISTORY_SIZE){
                    printf("Command number cannot be found. \n");
                    return 0;
                    
                }
                strcpy(inputBuffer,history[(temp-1)%HISTORY_SIZE]);
                
            }
            length = strlen(inputBuffer);
            
        }
        cmd_count++;
        strcpy(history[(cmd_count-1)%HISTORY_SIZE], inputBuffer);
        for (i = 0; i < length; i++) {
            if (inputBuffer[i] == '&') {
                inputBuffer[i] = '\0';
                *background = 1;
                --length;
                break;
            }
            
        }
    }
    
    /* examine every character in the inputBuffer */
    for (i = 0; i < length; i++) {
        switch (inputBuffer[i]){
            case ' ':
            case '\t' :               /* argument separators */
                if(start != -1){
                    args[ct] = &inputBuffer[start];    /* set up pointer */
                    ct++;
                }
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
                start = -1;
                break;
                
            case '\n':                 /* should be the final char examined */
                if (start != -1){
                    args[ct] = &inputBuffer[start];
                    ct++;
                }
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
                break;
                
            case '&':
                *background = 1;
                inputBuffer[i] = '\0';
                break;
                
            default :             /* some other character */
                if (start == -1)
                    start = i;
        }
    }
    args[ct] = NULL; /* just in case the input line was > 80 */
}

int main(void)
{
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2+1];/* command line (of 80) has max of 40 arguments */
    
    while (1){            /* Program terminates normally inside setup */
        background = 0;
        printf("SystemsIIShell->");
        fflush(0);
        setup(inputBuffer, args, &background);       /* get next command */
        
        pid_t child; /* process id for child */
        int status; /* status for execvp */
        
        child = fork(); /* create a child process*/
        
        if(child < 0){ /* if the child process didn't return 0, the fork is failed */
            printf("Fork failed! \n");
            
        }else if(child==0){ /* child process */
            if(inputBuffer[0]=='history' || inputBuffer[0] =='h'){
                viewHistory();
                return 0;
            }
            status = execvp(args[0],args);
            if(status !=0){
                printf("%s: command not found. \n", args[0]);
            }
            
        }else{ /* parent process */
            if(background == 0)
                waitpid(child,&background,0);
            
        }
        
        /* the steps are:
         (1) fork a child process using fork()
         (2) the child process will invoke execvp()
         (3) if background == 0, the parent will wait,
         otherwise returns to the setup() function. */
        
    }return 0;
}

CODE shellB.c
//
//  shell.c
//  CSE2431
//
//  Created by iqbal hasnan on 6/17/13.
//  Copyright (c) 2013 iqbal hasnan. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define HISTORY_SIZE 5 /*keep track of 5 most recent commands*/

int cmd_count; /*global to keep track of most recent commands entered*/
char history[HISTORY_SIZE][MAX_LINE]; /* global so it can be accessed in interrupt handler. */
char historyFileName[] = "./history";

/**
 * load history 
 */
int loadHistory(){
    
    int i = 0;
	char line[MAX_LINE];
	
	FILE *historyFile = fopen(historyFileName, "r");


        while(!feof(historyFile)) {
            fgets(line, MAX_LINE, historyFile);
            strcpy(history[i%HISTORY_SIZE], line);
            i++;
        }

	
    
    fclose(historyFile);
}

/**
 * save to historyFilanem when receive ctrl + c signal
 */
void saveHistory(){
    
    int i;
	
	FILE *historyFile = fopen(historyFileName, "w");
    
    /* Writes the history to hisFile */
    for (i = (cmd_count >= HISTORY_SIZE) ? cmd_count - HISTORY_SIZE:0;
         i < cmd_count; i++)
        fprintf(historyFile,"%s",history[i%HISTORY_SIZE]);
	fclose(historyFile);
}

/**
 * print command history
 */
void viewHistory()
{
    int i;
    
    if (cmd_count < 1)
        printf("No command history to show. \n");
    else {
        printf("\n\n");
        for (i = (cmd_count >= HISTORY_SIZE) ? cmd_count - HISTORY_SIZE:0;
             i < cmd_count; i++)
            printf("%d: %s\n",i+1,history[i%HISTORY_SIZE]);
    }
    
}


/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a
 * null-terminated string.
 */

int setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
    i,      /* loop index for accessing inputBuffer array */
    start,  /* index where beginning of next command parameter is */
    ct;     /* index of where to place the next parameter into args[] */
    
    int temp;
    
    ct = 0;
    
    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
    
    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0){
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }else{
        inputBuffer[length]='\0';
        if(inputBuffer[0]=='r'){
            if(inputBuffer[1]=='r'){
                if(cmd_count==0){
                    printf("No recent command can be found in the history. \n");
                    return 0;
                }
                strcpy(inputBuffer,history[(cmd_count)% HISTORY_SIZE]);
            }else{
                temp = atoi(&inputBuffer[1]);
                if(temp < 1 || temp > cmd_count || temp <= cmd_count -HISTORY_SIZE){
                    printf("Command number cannot be found. \n");
                    return 0;
                    
                }
                strcpy(inputBuffer,history[(temp-1)%HISTORY_SIZE]);
                
            }
            length = strlen(inputBuffer);
            
        }
        cmd_count++;
        strcpy(history[(cmd_count-1)%HISTORY_SIZE], inputBuffer);
        for (i = 0; i < length; i++) {
            if (inputBuffer[i] == '&') {
                inputBuffer[i] = '\0';
                *background = 1;
                --length;
                break;
            }
            
        }
    }
    
    /* examine every character in the inputBuffer */
    for (i = 0; i < length; i++) {
        switch (inputBuffer[i]){
            case ' ':
            case '\t' :               /* argument separators */
                if(start != -1){
                    args[ct] = &inputBuffer[start];    /* set up pointer */
                    ct++;
                }
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
                start = -1;
                break;
                
            case '\n':                 /* should be the final char examined */
                if (start != -1){
                    args[ct] = &inputBuffer[start];
                    ct++;
                }
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
                break;
                
            case '&':
                *background = 1;
                inputBuffer[i] = '\0';
                break;
                
            default :             /* some other character */
                if (start == -1)
                    start = i;
        }
    }
    args[ct] = NULL; /* just in case the input line was > 80 */
}

int main(void)
{
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2+1];/* command line (of 80) has max of 40 arguments */
    
    
    cmd_count = 0;
    
    /* set up the signal handler */
	struct sigaction handler;
	handler.sa_handler = saveHistory;
	sigaction(SIGINT, &handler, NULL);
    
    if( access( historyFileName, R_OK ) != -1 ) {
        loadHistory();
    }
    
    
    while (1){            /* Program terminates normally inside setup */
        background = 0;
        printf("SystemsIIShell->");
        fflush(0);
        setup(inputBuffer, args, &background);       /* get next command */
        
        pid_t child; /* process id for child */
        int status; /* status for execvp */
        
        child = fork(); /* create a child process*/
        
        if(child < 0){ /* if the child process didn't return 0, the fork is failed */
            printf("Fork failed! \n");
            
        }else if(child==0){ /* child process */
            if(inputBuffer[0]=='history' || inputBuffer[0] =='h'){
                viewHistory();
                return 0;
            }
            status = execvp(args[0],args);
            if(status !=0){
                printf("%s: command not found. \n", args[0]);
            }
            
        }else{ /* parent process */
            if(background == 0)
                waitpid(child,&background,0);
            
        }
        
        /* the steps are:
         (1) fork a child process using fork()
         (2) the child process will invoke execvp()
         (3) if background == 0, the parent will wait,
         otherwise returns to the setup() function. */
        
    }return 0;
}

