#include <stdio.h>
#include <stdlib.h>
#ifndef __USE_GNU
#define __USE_GNU

#include <ucontext.h>

#include <stdint.h>
#include <sys/ucontext.h>
#include <stdbool.h>
#include "tlib.h"

#define READY 1
#define RUNNING -1

/*
 * Committers
 *
 * Serhan Gürsoy
 * Ege Yosunkaya
 *
 */

//
//                       _oo0oo_
//                      o8888888o
//                      88" . "88
//                      (| -_- |)
//                      0\  =  /0
//                    ___/`---'\___
//                  .' \\|     |// '.
//                 / \\|||  :  |||// \
//                / _||||| -:- |||||- \
//               |   | \\\  -  /// |   |
//               | \_|  ''\---/''  |_/ |
//               \  .-\__  '-'  ___/-. /
//             ___'. .'  /--.--\  `. .'___
//          ."" '<  `.___\_<|>_/___.' >' "".
//         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
//         \  \ `_.   \_ __\ /__ _/   .-` /  /
//     =====`-.____`.___ \_____/___.-`___.-'=====
//                       `=---='
//
//
//     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    BUDDHA BLESS THIS CODE AND MAY IT BE BUG FREE
//               佛祖保佑         永无BUG
//

int id; // We need id counter. We should assign different id's.
int thread_count; // Lets keep the thread count in here.

// Ready_queue is the list of queues that created.
// Main context is the TCB that keeps our main context
// Current Thread keeps the current thread.
TCB *ready_queue,*mainContext, *currentThread;

int tlib_init (void)
{
    // Lets create our main thread.
    mainContext = (TCB *)malloc(sizeof(TCB));

    if (mainContext == NULL)
        return (TLIB_NOMEMORY);

    id = 0;   // Lets start id's from 1, hence we can increase that.
    thread_count = 0;   // Increase thread count. In this context, make it one, since our init is also a thread
    mainContext->t_di = id;  // Set first id to zero
    mainContext->next = NULL;  // It has no NULL


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
        //AYE AYE CAPTAIN
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
   // printf("\nWe are in stub. Arg is : %i\n" ,  arg);
     // currentThread->state == RUNNING;
    tstartf (arg); /* calling thread start function to execute */
    /*
        We are done with executing the application specified thread start
        function. Hence we can now terminate the thread
    */
    tlib_delete_thread(TLIB_SELF);
    printf("I believe I'm done :) I'm thread %i\n", currentThread->t_di);
    exit(0);
}


int tlib_create_thread(void (*func)(void *), void *param)
{
        TCB *temp = malloc(sizeof( TCB ) );
        temp->t_di = ++id;
        temp->state = READY;
        /* Create new stack */
        getcontext(&(temp->t_cont));
        temp->t_cont.uc_stack.ss_sp = malloc(TLIB_MIN_STACK);
        temp->t_cont.uc_stack.ss_size = TLIB_MIN_STACK;
        temp->t_cont.uc_stack.ss_flags = 0;
        temp->t_cont.uc_link = 0;
        //makecontext(&(temp->t_cont), (void *)stub , 2 , (void *)func , (int)param);
        greg_t *sp = (greg_t*)((uintptr_t) temp->t_cont.uc_stack.ss_sp
                                            + temp->t_cont.uc_stack.ss_size);
        sp -= 2;                                                    //Make room for 2 parameters
        temp->t_cont.uc_mcontext.gregs[REG_EIP] = (uintptr_t) stub; //Set EIP to stub
        temp->t_cont.uc_mcontext.gregs[REG_ESP] = (uintptr_t) sp;   //Set ESP to point top of the stack
        sp[0] = (uintptr_t) NULL;                                   //TRASH VALUE TO THE TRAMBOLINE ADDRESS
        sp[1] = (uintptr_t)func;                                    //Push Parameter 1 --> func
        sp[2] = (uintptr_t)param;                                   //Push Parameter 2 --> param


        /*ADD NEW THREAD TO READY QUEUE
         * */
        if (ready_queue == NULL)
            temp->t_cont.uc_link = 0;
        else {
            TCB *tracker = ready_queue;

            while (tracker->next != NULL)
                tracker = tracker->next;


        }

        // Insert thread to queue.
        insertThread(temp);

        thread_count++;

        return temp->t_di;
}


/*
 *  tlib_yield(int) is used to give CPU to another thread
 *  wantTid is the thread id that CPU will be given
 *  if wantTid == TLIB_ANY it means give the CPU to any other thread
 *  function will check if there is another thread in the queue and give CPU to the first "ready" thread
 *
 *
 *  if ready queue is empty -->> ?????????????????????????????? //TODO ?
 *
 */
int tlib_yield(int wantTid)
{
    bool first_time = TRUE;
    bool found=FALSE;
    // If we want to yield anything, then just yield to next. If It exists.. If it doesnt exists, just
    // go to the head of the ready queue.
    if (wantTid == TLIB_ANY) {
        printf("Yielding to the ANY!");
        if (ready_queue->next != NULL)
            wantTid = ready_queue->next->t_di;
        else
            wantTid = ready_queue->t_di;
    }else if (wantTid == TLIB_SELF) // Yielding to itself? Well.. Do nothing
        wantTid = currentThread->t_di;//wantTid = currentThread->t_di;
    else if (wantTid == currentThread->t_di)
        wantTid = currentThread->t_di;

    TCB *tmp = ready_queue; // Get the copy of ready queue
   // currentThread->state = RUNNING;
    for (int k = 0; k < thread_count ; k++) {  // Since we already know our thread count, we can iter through all threads to find our thread
       // printf("\n**We are in here : %i, Current thread: %i; We are looking for : %i **\n",tmp->t_di,currentThread->t_di, wantTid);
        if(tmp->t_di == wantTid) {
            getcontext(&currentThread->t_cont);
           // printf("We are in the want id and tmp match.\n");
            if(first_time == TRUE){
                currentThread->state = READY;
                tmp->state = RUNNING;
                first_time = FALSE;
                printf("\nCurrent thread %i and we are switching to %i Thread count %i\n",currentThread->t_di,tmp->t_di, thread_count);
                currentThread = tmp;
                found = true;
                setcontext(&currentThread->t_cont);
            }else {
                printf("Am i here");
                currentThread = tmp;
                tmp->state=READY;
                //currentThread->state = RUNNING;
            }
        }
        tmp = tmp->next;
    }

    //currentThread->state = RUNNING;
    if (found) {
        //currentThread->state = RUNNING;
        return (currentThread->t_di);
    }
    else {
        // No thread found with given id.
        currentThread->state =RUNNING;
        printf("Couldn't find %i. Its finished or never existed\n", wantTid);

        return (TLIB_INVALID);
    }
}


int tlib_delete_thread(int tid)
{

    if (tid == TLIB_SELF) {

        printf("\nXXXXXXXXDeleted tid %iXXXXXXX\n", currentThread->t_di);
        int thid = currentThread->t_di;

        TCB *tracker = ready_queue;

        while (tracker->next->t_di != thid)
            tracker = tracker->next;

        TCB *deletor = tracker->next;
        tracker->next = tracker->next->next;
        //currentThread = tracker;
        thread_count--;
        deletor->t_cont.uc_stack.ss_size = 0;
        deletor->t_cont.uc_link = 0;
        free(deletor->t_cont.uc_stack.ss_sp);
        free(deletor);
        if(ready_queue->next != NULL)           //NEW
            currentThread = ready_queue->next;
        else
            currentThread = ready_queue;
        //tlib_yield(TLIB_ANY);
    } else if (tid == TLIB_ANY) { // Does that even exist? If so
        TCB *deletor = ready_queue;
        ready_queue = ready_queue->next;
        deletor->t_cont.uc_stack.ss_size = 0;
        deletor->t_cont.uc_link = 0;
        free(deletor->t_cont.uc_stack.ss_sp);
        free(deletor);
        thread_count--;
        //tlib_yield(TLIB_ANY);
    } else { // Delete the node

        printf("Deleted tid %i", tid);
        int thid = tid;

        TCB *tracker = ready_queue;

        while (tracker->next->t_di != thid)
            tracker = tracker->next;

        TCB *deletor = tracker->next;
        tracker->next = tracker->next->next;
        //currentThread = tracker;
        thread_count--;
        deletor->t_cont.uc_stack.ss_size = 0;
        deletor->t_cont.uc_link = 0;
        free(deletor->t_cont.uc_stack.ss_sp);
        free(deletor);
        tlib_yield(TLIB_ANY);
    }
    return (TLIB_ERROR);
}


#endif



