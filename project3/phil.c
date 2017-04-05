//
// Created by EGE YOSUNKAYA on 24.03.2017.


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <semaphore.h>
#include <time.h>
#include <math.h>



//GLOBAL VARIABLES
int NUM_PHIL;
int MAX_THINK;
int MIN_THINK;
int MAX_EAT;
int MIN_EAT;
char* distribution;
sem_t s;
sem_t *locks;
int *state;
int EAT_COUNT;
double *hungry_times;
double *clocks;
double *clocks_per_execution;

#define PHIL_COUNT NUM_PHIL
#define MAX_PHIL_COUNT 27
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define RIGHT (num + 1) % PHIL_COUNT
#define LEFT  (num + PHIL_COUNT - 1) % PHIL_COUNT
#define EXPONENTIAL "exponential"
#define UNIFORM "uniform"





//RETURNS MILISECOND
double random_time(char* dist ,int MIN , int MAX){
   if(strcmp(dist, EXPONENTIAL) == 0) {
       double z;
       double exp_value;
       do{
            do {
                z = rand() % 100;
                z = z / 100.0;

            } while (z == 0 || z == 1);


           exp_value = -((MAX + MIN) / 2) * log(z);
       } while (exp_value >= MAX || exp_value <= MIN);

       return (exp_value);

   } else if (strcmp(dist , UNIFORM) == 0){

       int range = MAX-MIN;
       double z;
       do {
           z = rand() % range;
           z += MIN;

       }while(z == 0 || z == 1);
       return z;

   }else {
       exit(1);
   }
}



void test(int num){
    if(state[num] == HUNGRY &&
       state[LEFT] != EATING &&
       state[RIGHT] != EATING)
    {
        state[num] = EATING;
        printf("philosopher %d is EATING \n" , num + 1);
        sem_post(&locks[num]);

    }
}

void take_fork(int num){
    sem_wait(&s);
    state[num] = HUNGRY;

    printf("philosopher %d is HUNGRY \n", num + 1);
    test(num);
    sem_post(&s);
    sem_wait(&locks[num]);
}

void put_fork(int num){
    sem_wait(&s);
    state[num] = THINKING;
    printf("philosopher %d is THINKING \n" , num + 1);
    test(LEFT);
    test(RIGHT);
    sem_post(&s);
}

void *foo(void *arg){
    int count = 0;
    int *i = arg;

    do{

        usleep(random_time(distribution , MIN_THINK ,MAX_THINK) * 1000); // THINK

        //CLOCK START
        clocks[(*i)] = clock();
        take_fork(*i);
        //CLOCK END
        clocks[*i] = clock() - clocks[*i];
        hungry_times[*i] = hungry_times[*i] + (clocks[*i]/(double)CLOCKS_PER_SEC * 1000);
        int num = (*i * 100) + count;
        clocks_per_execution[num] = (clocks[*i]/(double)CLOCKS_PER_SEC * 1000);
        usleep(random_time(distribution , MIN_EAT ,MAX_EAT) * 1000); // EAT
        put_fork(*i);

        count++;
    }while(count != EAT_COUNT);

}

int *phil_num;

int main(int argc, char** argv) {
    //RANDOM NUMBER SEED
    unsigned int seed = (unsigned int) time(NULL);
    srand(seed);

    //GET INPUTS FROM ARGS
    NUM_PHIL = atoi(argv[1]);
    MIN_THINK = atoi(argv[2]);
    MAX_THINK = atoi(argv[3]);
    MIN_EAT = atoi(argv[4]);
    MAX_EAT = atoi(argv[5]);
    distribution = argv[6];
    EAT_COUNT = atoi(argv[7]);

    //ALLOCATE GLOBAL VARIABLES
    locks = malloc(sizeof(sem_t) * NUM_PHIL);
    state = malloc(sizeof(int) * NUM_PHIL);
    phil_num = malloc(sizeof(int) * NUM_PHIL);
    hungry_times = malloc(sizeof(double) * NUM_PHIL);
    clocks = malloc(sizeof(double) * NUM_PHIL);
    clocks_per_execution = malloc(sizeof(double) * NUM_PHIL * 100);

    //SET HUNGRY TIMES TO ZERO
    for(int i = 0; i < NUM_PHIL; i++){
        hungry_times[i] = 0;
    }

    pthread_t thread_list[PHIL_COUNT];
    //INITIALIZE MUTEX LOCK SEMAPHORE
    sem_init(&s , 1 , 1 );
    //INITIALIZE FORK SEMAPHORES
    for(int i = 0; i < PHIL_COUNT; i++) {
        sem_init(&locks[i], 0, 0);
        phil_num[i] = i;
        state[i] = THINKING;
    }



    for(int k = 0; k < PHIL_COUNT; k++){
        pthread_create(&thread_list[k], NULL , foo , &phil_num[k]);
        printf("Philosopher %d is THINKING \n" , k + 1);

    }

    /*do {
        for (int num = 0; num < PHIL_COUNT; num++) {
            sem_wait(&s);
            int right = RIGHT;
            int left = LEFT;
            if ((state[num] == EATING && state[RIGHT] == EATING) || (state[num] == EATING && state[LEFT] == EATING)) {
                printf("%s \n" , "We Fucked up guysss!");
            }
            sem_post(&s);
        }
    }while(1);*/ // TEST CODE FOR DEADLOCKS



    int status;
    for(int i = 0; i < PHIL_COUNT; i++){
        status = pthread_join(thread_list[i] , NULL);
        if(status != 0)
            exit(5);
    }
    for(int i = 0; i < NUM_PHIL; i++) {
        printf("Philosopher %d duration of hungry state: %.6f ms. \n", (i + 1), hungry_times[i]);
    }

    //TEST CODE FOR REPORT - AVERAGE and STANDART DEVIATION
    /*int num;
    double averages[NUM_PHIL];
    double standart_devs[NUM_PHIL];
    for(int i = 0; i < NUM_PHIL; i++){
        averages[i]=  hungry_times[i] / 100;
        for(int j = 0; j < 100; j++){
             num = i * 100 + j;
            standart_devs[i] = standart_devs[i] + ((averages[i] - clocks_per_execution[num])*(averages[i] - clocks_per_execution[num])/100);
        }
        printf("philosopher %d Average: %.6f ms. -- Standart Dev: %.10f ms. \n" , (i+1) , averages[i] , standart_devs[i]);
    }
    */

    return 0;
}

