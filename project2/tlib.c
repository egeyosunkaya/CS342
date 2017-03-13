#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <assert.h>
#define __USE_GNU
#include <ucontext.h>
#include <sys/ucontext.h>
#include <stdint.h>
#include "tlib.h"

#define READY 1
#define RUNNING -1

/*
 * Committers
 *
 * Serhan Gürsoy
 * Ege Yosunkaya
 *
 *
 *
 *
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
static volatile boolean returned;





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
    char *stack = malloc(TLIB_MIN_STACK);


    // Check whether we have space or not.
    if (TLIB_MAX_THREADS > thread_count) {

        // Create temp malloc
        TCB *temp = (TCB *) malloc(sizeof(TCB));

        /* Increment ID  */
        id++;

        /* Set next to NULL. */
        temp->next = NULL;
        temp->t_di = id;
        temp->t_state = READY;


        /*Instruction Pointer Set*/
        temp->t_cont.uc_mcontext.gregs[REG_EIP] = (greg_t) &stub;
        /* Stack Create */
        temp->t_cont.uc_stack.ss_sp = stack;
        temp->t_cont.uc_stack.ss_size = sizeof(stack);

        /* Stack Partion */
        temp->t_cont.uc_mcontext.gregs[REG_ESP] = (greg_t) temp->t_cont.uc_stack.ss_sp;
        temp->t_cont.uc_mcontext.gregs[REG_ESI] = (greg_t) param;


        /*ADD NEW THREAD TO READY QUEUE
         * */
        if (ready_queue == NULL)
            temp->t_cont.uc_link = 0;
        else {
            TCB *tracker = ready_queue;

            while (tracker->t_di != id - 1)
                tracker = tracker->next;

         //   printf("Thread id %i   successor id  %i ", temp->t_di, tracker->t_di);

        }

        // Insert it to queue.
        getcontext(&(temp->t_cont));
        insertThread(temp);
        thread_count++;

        return temp->t_di;
    }
    else
        return (TLIB_ERROR);
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

    //boolean found = FALSE; // Added variable to finish for loop if a thread is found

  //  printf("IN here. Thread count %i", thread_count);
    for (int k = 0; k < thread_count ; k++) {  // Since we already know our thread count, we can iter through all threads to find our thread
        if(tmp->t_di == wantTid) {
            //found = TRUE;
            // That's the id we want to start

            getcontext(&currentThread->t_cont);

            if (currentThread->t_state == RUNNING) {
                // We know our current running thread. It is tmp. Lets save it to our node. And start executing next item
                currentThread->t_state = READY;
                //printf("\nIN THREAD %i", currentThread->t_di);

                setcontext(&currentThread->t_cont);
            }else {
                currentThread = tmp;

              //  printf("IN LOL %i", tmp->t_di);
                currentThread->t_state = RUNNING;
                currentThread->t_cont.uc_mcontext.gregs[REG_EIP] = &stub;
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

