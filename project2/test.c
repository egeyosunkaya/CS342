#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "tlib.h"

#define OUT1 "out1"
#define OUT2 "out2"
#define OUT3 "out3"
#define OUT4 "out4"

#define NCASES 4
#define MAXTHREADS 3


void log_line(const char *path, const char *format, ...) {
  FILE *fp;
  va_list vargs;

  va_start(vargs, format);
  fp = fopen(path, "a");

  vfprintf(fp, format, vargs);
  fprintf(fp, "\n");

  fclose(fp);
  va_end(vargs);  
}


void *test1(void *id) {
  log_line(OUT1, "Thread is active");
  
  return NULL;
}


void *test2(void *id) {
  int i;

  for (i=0; i < 2; i++) {
    log_line(OUT2, "Thread %d is active: %d", (int) id, i);

    tlib_yield(TLIB_ANY);
  }

  log_line(OUT2, "Thread %d is active: %d", (int) id, i);

  return NULL;
}


void *test3_3(void *id) {
  log_line(OUT3, "Thread 3 is active");
  
  return NULL;
}


void *test3_2(void *id) {
  int tid;
  
  tid = tlib_create_thread((void *)&test3_3, (void *)(0));
  
  log_line(OUT3, "Thread 2 created thread");
  
  tlib_yield(tid);

  log_line(OUT3, "Thread 2 is active");
  
  return NULL;
}


void *test3_1(void *id) {
  int tid;
  
  tid = tlib_create_thread((void *)&test3_2, (void *)(0));
  
  log_line(OUT3, "Thread 1 created thread");
  
  tlib_yield(tid);

  log_line(OUT3, "Thread 1 is active");

  return NULL;
}


void *test4(void *tids) {
  tlib_delete_thread(((int *) tids)[0]);
  log_line(OUT4, "Thread 2 deleted thread 1");
  
  tlib_delete_thread(((int *) tids)[2]);
  log_line(OUT4, "Thread 2 deleted thread 3");

  tlib_yield(TLIB_ANY);
  log_line(OUT4, "Thread 2 is active");

  return NULL;
}


int main(int argc, char **argv) {
  char *files[NCASES+1] = {"", OUT1, OUT2, OUT3, OUT4};
  int caseno;
  int tid[MAXTHREADS];
  int i;

  if (argc != 2) {
    fprintf(stderr, "Exactly one argument is required\n");
    fprintf(stderr, "Usage: test caseno");

    return 1;
  }

  tlib_init();

  caseno = atoi(argv[1]);
  remove(files[caseno]);

  /* Case 1: Simple case
     1. Create 1 thread
     2. Yield to it
     3. Thread returns
   */

  if (caseno == 1) {
    tid[0] = tlib_create_thread((void *)&test1, (void *)(0));
    tlib_yield(tid[0]);
  }

  /* Case 2: Yield to any
     1. Create 3 threads
     2. All threads yield to any, each twice
     3. All threads return
   */

  else if (caseno == 2) {
    for (i=0; i < MAXTHREADS; i++) {
      tid[i] = tlib_create_thread((void *)&test2, (void *)(i));
    }

    for (i=0; i < 6; i++) {
      tlib_yield(TLIB_ANY);
      log_line(OUT2, "Main thread is active");
    }
  }

  /* Case 3: Threads creating threads
     1. Create 1 thread
     2. Yield to it
     3. Thread 1 creates another thread and yields to it
     4. Thread 2 creates another thread and yields to it
     5. Thread 3 returns
     6. Threads 1 and 2 return when they are scheduled
   */

  else if (caseno == 3) {
    tid[0] = tlib_create_thread((void *)&test3_1, (void *)(0));
    tlib_yield(tid[0]);

    for (i=0; i < 2; i++) {
      tlib_yield(TLIB_ANY);
    }
  }

  /* Case 4: One thread deleting others before they were scheduled
     1. Create 3 threads
     2. Yield to Thread 2, passing tid array as argument
     3. Thread 2 deletes Threads 1 and 3
     4. Thread 2 yields to any (main thread)
     5. Main thread yields to any (Thread 2)
     6. Thread 2 returns
   */

  else if (caseno == 4) {
    for (i=0; i < 3; i++) {
      tid[i] = tlib_create_thread((void *)&test4, (void *)(tid));
    }

    tlib_yield(tid[1]);

    log_line(OUT4, "Main thread is active");
    tlib_yield(TLIB_ANY);
  }

  return 0;
}
