





#include <assert.h>
#include <stdio.h>
#include "tlib.h"

void *foo(void *v)
{
    int count = 0;
//	while (count < 100) {
//		printf ("thread %d is running\n", (unsigned int)v);
//		if (count % 10 == 0) {
//			tlib_yield (count);
//		}
//		count++;
//	}

    printf ("thread %d is finished , exiting...\n", (unsigned int)v);
    tlib_yield((unsigned int)v + 1);


    printf("Ben thread %d , bana döndüler reis \n" ,(unsigned int) v) ;
    return (NULL);
}


int main(int argc, char **argv)
{
    int tid[1000];
    int c = 0;
    int i;

    tlib_init ();

    for (i = 0; i < 6; ++i) {
        tid[i] = tlib_create_thread ((void *)&foo, (void *)(i+1));
        printf ("thead %d created\n", (int) tid[i]);

    }

    //while (c < 100) {
    printf ("thread 0 is running\n");
    //if (c % 10 == 0) {
    tlib_yield (1);
    //}
    //c++;
    //}
    return 0;
}










/* #include <assert.h>
#include <stdio.h>
#include "tlib.h"

void *foo(void *v)
{
	int count = 0; 
	while (count < 10) {
		printf ("thread %d is running\n", (unsigned int)v);
		if (count % 100 == 0) {
		//	 tlib_yield (TLIB_SELF);
		}
		count++; 
	}
	return (NULL); 
}


int 
main(int argc, char **argv)
{
	int tid[10];
	int c = 0;
	int i; 
	
	tlib_init ();

	for (i = 0; i < 10; ++i) {
		tid[i] = tlib_create_thread ((void *)&foo, (void *)(i+2));
		printf ("thead %d created\n",  tid[i]);
	}
		
	while (c < 1000) {
	  	printf ("thread 1 is running\n");
		if (c % 100 == 0 && c > 99) {
            printf("Hello kidoman %i " , tid[c/100]);
			 tlib_yield (tid[(c / 100)]);
		}
		c++;
	}
	return 0;
}
*/