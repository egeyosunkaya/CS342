#include <stdio.h>
#include <stdlib.h>
#ifndef __USE_GNU
#define __USE_GNU

#include <ucontext.h>

#include <stdint.h>
#include <sys/ucontext.h>
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
    printf("\nWe are in stub. Arg is : %i\n" ,  arg);

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
        TCB *temp = malloc(sizeof( TCB ) );
        temp->t_di = ++id;
        temp->state = RUNNING;
        /* Create new stack */
        getcontext(&(temp->t_cont));
        temp->t_cont.uc_stack.ss_sp = malloc(TLIB_MIN_STACK);
        temp->t_cont.uc_stack.ss_size = TLIB_MIN_STACK;
        temp->t_cont.uc_stack.ss_flags = 0;
        temp->t_cont.uc_link = 0;
        makecontext(&(temp->t_cont), (void *)stub , 2 , (void *)func , (int)param);
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
    if (wantTid == TLIB_ANY)
        if (ready_queue->next != NULL)
            wantTid = ready_queue->next->t_di;
       else
            wantTid = ready_queue->t_di;

    TCB *tmp = ready_queue; // Get the copy of ready queue

    for (int k = 0; k < thread_count ; k++) {  // Since we already know our thread count, we can iter through all threads to find our thread
        if(tmp->t_di == wantTid) {
            getcontext(&currentThread->t_cont);
            if(tmp->state == RUNNING){
                tmp->state = READY;
                currentThread = tmp;
                setcontext(&currentThread->t_cont);
            }

        }
        tmp = tmp->next;
    }


    return (TLIB_SUCCESS);

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

#endif