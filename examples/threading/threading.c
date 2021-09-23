#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

//int thread_num=0;

//pthread_t thread;
// pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_init(mutex_lock);


//delays by ms_wait milliseconds
//@param int
//returns int
void wait(int ms_wait)
{
   
    clock_t time;
    //return the time in us
    time = clock();
    //error check, must return 0
    if(time == -1)
    {
        ERROR_LOG("\nclock");
    }
    while(((clock() - time)/1000) != ms_wait); //seconds to ms
}



void* threadfunc(void* thread_param)
{
    struct thread_data *func_data = (struct thread_data *) thread_param;
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    printf("thread_id = %lu\n", func_data->thread_id);
    pthread_mutex_init(func_data->mutex_lock, NULL);
    // func_data->mutex_lock = PTHREAD_MUTEX_INITIALIZER;
    wait(func_data->obtain_wait);
    pthread_mutex_lock(func_data->mutex_lock);
    wait(func_data->release_wait);
    pthread_mutex_unlock(func_data->mutex_lock); 
     
    //ERROR_LOG("thread area\n"); 
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     * 
     * See implementation details in threading.h file comment block
     */
     
     //allocating dynamic memory to the data structure
     struct thread_data *data = (struct thread_data *) malloc(sizeof(struct thread_data));
     if(data == NULL)
        ERROR_LOG("mallock\n");
      
      data->thread_id = *thread;
      data->mutex_lock =  mutex;
      data->obtain_wait = wait_to_obtain_ms;
      data->release_wait = wait_to_release_ms;

      printf("thread_id starter func= %lu\n", data->thread_id);
      
    
     int create;
     create = pthread_create(thread, NULL, threadfunc, (void *)data);
     if(create != 0)
     {
         ERROR_LOG("thread not created\n");
         //data->thread_complete_success = false;
     }
     else
     {
        ERROR_LOG("thread executed successfully\n"); 
     }
     //return data->thread_complete_success;
     pthread_exit((void *)data);
     if(data->thread_complete_success == true)
     {
         free(data);
         return true;
     }
     else
     {
         free(data);
         return  false;
     }
}
