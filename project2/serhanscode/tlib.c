#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <assert.h>

#include <ucontext.h>
#include <sys/ucontext.h>
#include "tlib.h"

#define READY 1
#define RUNNING -1

int id; // We need id counter. We should assign different id's.
int thread_count; // Lets keep the thread count in here.

// Ready_queue is the list of queues that created.
// Main context is the TCB that keeps our main context
// Current Thread keeps the current thread.
TCB *ready_queue,*mainContext, *currentThread;

// Stack initialization for our threads. We will fill it while creating them
char ThreadStack[TLIB_MIN_STACK];



int tlib_init (void)
{
    // Lets create our main thread.
    mainContext = (TCB *)malloc(sizeof(TCB));

    if (mainContext == NULL)
        return (TLIB_ERROR);

    id = 0;   // Lets start id's from 1, hence we can increase that.
    thread_count = 0;   // Increase thread count. In this context, make it one, since our init is also a thread
    mainContext->t_di = id;  // Set first id to zero
    mainContext->next = NULL;  // It has no NULL
    mainContext->t_state = RUNNING; // Set it to ready.

    // Set its context to main context's current content.
    getcontext(&mainContext->t_cont);

    // NO CONTENT? Something happened for sure..
    if (&mainContext->t_cont == NULL)
       return(TLIB_NONE);

    // Our ready queue will be main also.
    ready_queue = mainContext;
    currentThread = ready_queue;

    // Show debug method
    printf("Initialization successful!\n");
    return (TLIB_SUCCESS);
}

void insertThread(TCB *thr) {
    TCB *tracker;
    if (ready_queue == NULL) {
        // Well, if everything goes right, it can't be null. However, we need to make it sure, aye?
        ready_queue = thr;
    } else {
        // Go until next becomes null. When its null, just damn add it to next.
        tracker = ready_queue;
        while(tracker->next != NULL)
            tracker = tracker->next;

        tracker->next = thr;
        //printf("Inserted! Thread id %i Total thread count is %i\n" , thr->t_di, (thread_count));
    }
}

/* implementation of stub is already given below */
void stub (void (*tstartf)(void *), void *arg)
{

    //printf("\nWe are in stub. Arg is : %i\n" , arg);

    tstartf (arg); /* calling thread start function to execute */
    /*
        We are done with executing the application specified thread start
        function. Hence we can now terminate the thread
    */
    tlib_delete_thread(TLIB_SELF);
    exit(0);
}


int tlib_create_thread(void (*func)(void *), void *param)
{
    // Check whether we have space or not.
    if (TLIB_MAX_THREADS > thread_count) {

        // Create temp malloc
        TCB *temp = (TCB *) malloc(sizeof(TCB));
        id++;
        // Set next to NULL.
        temp->next = NULL;
        temp->t_di = id;
        temp->t_state = READY;
        getcontext(&temp->t_cont);
        temp->t_cont.uc_stack.ss_sp = ThreadStack; // Stack pointer, point it to our threadStack :)
        temp->t_cont.uc_stack.ss_size = sizeof(ThreadStack); // Set size

        if (ready_queue == NULL)
            temp->t_cont.uc_link = 0;
        else {
            TCB *tracker = ready_queue;

            while (tracker->t_di != id - 1)
                tracker = tracker->next;

         //   printf("Thread id %i   successor id  %i ", temp->t_di, tracker->t_di);
            temp->t_cont.uc_link = &tracker->t_cont;
        }
        // Set parameters and set function
        temp->arg = param;
        temp->func = func;
        // Insert it to queue.
        insertThread(temp);
        thread_count++;

        return temp->t_di;
    }
    else
        return (TLIB_ERROR);
}


int tlib_yield(int wantTid)
{
    if (wantTid == TLIB_ANY)
        if (ready_queue->next != NULL)
        wantTid = ready_queue->next->t_di;
       else
            wantTid = ready_queue->t_di;

    TCB *tmp = ready_queue; // Lets get the copy of our ready queue
  //  printf("IN here. Thread count %i", thread_count);
    for (int k = 0; k < thread_count ; k++) {  // Since we already know our thread count, we can iter through all threads to find our thread
        if(tmp->t_di == wantTid) {
            // Thats the id we want to start
            getcontext(&currentThread->t_cont);
            if (currentThread->t_state == RUNNING) {
                // We know our current running thread. It is tmp. Lets save it to our node. And start executing next item
                currentThread->t_state = READY;
               // printf("\nIN THREAD %i", currentThread->t_di);
                setcontext(&currentThread->t_cont);
            }else {
                currentThread = tmp;
                //TCB *tracker = ready_queue;

                //while(tracker->t_di != wantTid - 1)
                 //   tracker = tracker->next;

                //currentThread->t_cont.uc_link = &tracker->t_cont;

              //  printf("IN LOL %i", tmp->t_di);
                currentThread->t_state = RUNNING;
                stub(tmp->func, tmp->arg);
            }
        }
        tmp = tmp->next;
    }
    return (TLIB_SUCCESS);

/*
    if(getcontext(current->ucontext) == CONTEXT_ERROR)
        return (TLIB_ERROR);
    else{
        current = &(TCBLIST[wantTid]);
        if(current->ucontext != NULL){
            if(isRunning == RUNNING){
                isRunning = READY;
                current->prev_id = temp;
                setcontext((current->ucontext));
            }
        } else {
            current->prev_id = temp;
            current->ucontext = (ucontext_t* )malloc( sizeof( ucontext_t) );
            stub(current->start_func, current->param);
        }
*/

        //  }else
    //    return (TLIB_ERROR);
}


int tlib_delete_thread(int tid)
{
    if (tid == TLIB_SELF) {

       // printf("Deleted tid %i", currentThread->t_di);
        int thid = currentThread->t_di;

        TCB *tracker = ready_queue;

        while (tracker->next->t_di != thid)
            tracker = tracker->next;

        TCB *deletor = tracker->next;
        tracker->next = tracker->next->next;
        //currentThread = tracker;
        thread_count--;
        free(deletor);
        tlib_yield(TLIB_ANY);
    }
    return (TLIB_ERROR);
}

