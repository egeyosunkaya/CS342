

/*
 *
 * TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE
 * ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST
 * TE ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE T TE ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE T
 * E TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZO E TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZO
 * TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE
 * ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST
 * TE ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE T TE ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE T
 * E TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZO E TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZONE TEST ZO
 *
*/


/*

#include <stdio.h>
#include "tlib.h"

void *foo(void *v)
{
    printf("%s \n" , "FOO GİRDİİ");
    if(v == NULL)
        printf("%s \n" , "NULLLLLLLL");
    //int count = 0;
	//while (count < 100) {
		printf ("thread %d is running\n", (unsigned int)v);
	//	if (count % 10 == 0 && count > 9) {
			tlib_yield (( (int)v )+ 1);
	//	}
	//	count++;
	//}

    return (NULL);
}


int main(int argc, char **argv)
{
    int tid[10];
    int c = 0;
    int i;

    tlib_init ();

    for (i = 1; i < 14; ++i) {
        tid[i] = tlib_create_thread ((void *)&foo, (void *)(i));
        printf ("thead %d created\n", (int) tid[i]);

    }

   // while (c < 100) {
    //    printf ("thread 0 is running\n");
     //   if (c % 10 == 0 && c > 9) {
            tlib_yield (0);
      //  }
    //c++;
    //}
    return 0;
}


*/







 #include <assert.h>
#include <stdio.h>
#include "tlib.h"

void *foo(void *v)
{
    /*
	int count = 0;
	while (count < 100) {
		printf ("thread %d is running with count : %i\n", (unsigned int)v, count);
		if (count % 10 == 0 && count > 10) {
            printf("\nYielding To %i\n", count/10);
			 tlib_yield (count/10);
           printf("Inside Foo... Means that yield ended successfully.\n");
		}
		count++; 
	}*/

    printf("Thread %d running! Lets yield to other!", (unsigned int) v);
    //tlib_delete_thread(v);
    tlib_yield(v+1);
	return (NULL); 
}


int 
main(int argc, char **argv)
{
	int tid[10];
	int c = 0;
	int i; 
	
	tlib_init ();

    /*
	for (i = 0; i < 10; ++i) {
		tid[i] = tlib_create_thread ((void *)&foo, (void *)(i+1));
		printf ("thead %d created\n",  tid[i]);
	}
		
	while (c < 200) {
	  	printf ("thread 1 is running\n");
		if (c % 100 == 0 && c > 99) {
            printf("Hello kidoman %i " , tid[c/100]);
			 tlib_yield (tid[(c % 100)]);
		}
		c++;
	}*/

    tid[0] = tlib_create_thread((void *)&foo, (void *)(1));
    tid[1] = tlib_create_thread((void *)&foo, (void *)(2));
    tid[2] = tlib_create_thread((void *)&foo, (void *)(3));
    tid[3] = tlib_create_thread((void *)&foo, (void *)(4));
    tid[4] = tlib_create_thread((void *)&foo, (void *)(5));
    tid[5] = tlib_create_thread((void *)&foo, (void *)(6));

    //tlib_yield(1);
    tlib_delete_thread(5);
    //tlib_delete_thread(4);
    //tlib_delete_thread(3);
    //tlib_delete_thread(2);
    //tlib_delete_thread(1);
    tlib_yield(3);

	return 0;
}

