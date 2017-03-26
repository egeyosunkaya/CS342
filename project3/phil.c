//
// Created by student on 24.03.2017.


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <semaphore.h>
#include <time.h>


#define PHIL_COUNT 5
#define AVAILABLE 1
#define BUSY 0
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define RIGHT (num + 1) % PHIL_COUNT
#define LEFT  (num + PHIL_COUNT - 1) % PHIL_COUNT
#define EXPOTENTIAL "expotential"
#define UNIFORM "uniform"
#define NORMAL "stdnormal"

#define randnum(min ,max) \
            (rand() % (int)(((max) + 1) - (min))) + (min))

/*int sem_wait(Semaphore *s) {
    pthread_mutex_lock(s->mutex);
    s->value--;
    if(s->value < 0){
        do {
            pthread_cond_wait(s->cond , s->mutex);
        }while(s->wakeup < 1);

        s->wakeup--;
    }
    pthread_mutex_unlock(s->mutex);
}

int sem_signal(Semaphore *s) {
    pthread_mutex_lock(s->mutex);
    s->value++;
    if(s->value <= 0) {
        s->wakeup++;
        pthread_cond_signal(s->cond);
    }

    pthread_mutex_unlock(s->mutex);
}

int sem_init(Semaphore *s){
    pthread_mutex
}*/

sem_t s;
sem_t locks[PHIL_COUNT];
int state[PHIL_COUNT];
int EAT_COUNT;



void test(int num){
    if(state[num] == HUNGRY &&
       state[LEFT] != EATING &&
       state[RIGHT] != EATING)
    {
        state[num] = EATING;
        printf("Phil %d is EATING \n" ,num + 1);
        sem_post(&locks[num]);

    }
}

void take_fork(int num){
    sem_wait(&s);
    state[num] = HUNGRY;
    printf("Phil %d is HUNGRY\n", num + 1);
    test(num);
    sem_post(&s);
    sem_wait(&locks[num]);
}

void put_fork(int num){
    sem_wait(&s);
    state[num] = THINKING;
    printf("Phil %d is THINKING \n" , num + 1);
    test(LEFT);
    test(RIGHT);
    sem_post(&s);
}

void *foo(void *arg){
    int *i = arg;
    do {

        usleep(500); //THINK

        take_fork(*i);

        usleep(100); // EAT

        put_fork(*i);

    }while(1);
}

int phil_num[PHIL_COUNT];

int main(int argc, char** argv) {
    srand(time(NULL)); // Seed for random

    EAT_COUNT = atoi(argv[6]);
    char *distribution = argv[5];
    int eat_min = atoi(argv[3]);
    int eat_max = atoi(argv[4]);
    int think_min = atoi(argv[1]);
    int think_max = atoi(argv[2]);


    printf("%d \n" , EAT_COUNT);
    printf("%d \n" , eat_min);
    printf("%d \n" , eat_max);
    printf("%d \n" , think_min);
    printf("%d \n" , think_max);
    printf("%s \n" , distribution);


    pthread_t thread_list[PHIL_COUNT];
    sem_init(&s , 1 , 1 );

    for(int i = 0; i < PHIL_COUNT; i++) {
        sem_init(&locks[i], 0, 0);
        phil_num[i] = i;
        state[i] = THINKING;
    }





    for(int k = 0; k < PHIL_COUNT; k++){
        pthread_create(&thread_list[k], NULL , foo , &phil_num[k]);
        printf("Phil %d is THINKING \n" , k + 1);

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
    }while(1);*/

    int status;
    for(int i = 0; i < PHIL_COUNT; i++){
        status = pthread_join(thread_list[i] , NULL);
        if(status != 0)
            exit(5);
    }

    return 0;
}

