#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_message_function( void *ptr );
extern void *print_message_function_lib(void *);

main()
{
     pthread_t thread1, thread2, thread3;
     char *message1 = "Thread 1";
     char *message2 = "Thread 2";
     char *message3 = "Thread 3";
     int  iret1, iret2, iret3;
    /* Create independant threads each of which will execute function */

     iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) message1);
     iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) message2);
     iret3 = pthread_create( &thread3, NULL, print_message_function_lib, (void*) message3);

     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */
     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL); 
     pthread_join( thread3, NULL); 

     printf("Thread 1 returns: %d\n",iret1);
     printf("Thread 2 returns: %d\n",iret2);
     printf("Thread 3 returns: %d\n",iret3);
     exit(0);
}

// Ceci est le test
// Jai rajoute une petite func dans le heap allocator
// malloc_dbgpid_set/get
void *print_message_function( void *ptr )
{
  char *message;

  // Here uncomment this to register the debugger pid and select a new arena for it
  if (malloc_dbgpid_get() == 0)
    malloc_dbgpid_set(getpid());

  while (1)
  {
    message = malloc(42);
    printf("%s: (pid = %u %s) message addr %08X \n", 
	   ptr, getpid(), 
	   (getpid() == malloc_dbgpid_get() ? "DEBUGGER" : "OTHER"),
	   message);
    //free(message);
    sleep(1);
  }
}
