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


//pthread_t thread;
//pthread_mutex_t mutex;


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
    printf("thread check\n");
    pthread_mutex_init(&func_data->mutex_lock, NULL);
    wait(func_data->obtain_wait);
    //ERROR_LOG("obtain wait; %d\n", data->obtain_wait); 
    pthread_mutex_lock(&func_data->mutex_lock);
    wait(func_data->release_wait);
    pthread_mutex_unlock(&func_data->mutex_lock); 
     
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
     
     struct thread_data *data = malloc(sizeof(struct thread_data));
     if(data == NULL)
        ERROR_LOG("mallock\n");
      data->thread_1 = *thread;
      data->mutex_lock =  *mutex;
      data->obtain_wait = wait_to_obtain_ms;
      data->release_wait = wait_to_release_ms;

      

     int create;
     create = pthread_create(thread, NULL, threadfunc, data);
     if(create != 0)
     {
         ERROR_LOG("thread not created\n");
         data->thread_complete_success = false;
     }
     else
     {
        data->thread_complete_success = true;
        ERROR_LOG("thread executed successfully\n"); 
     }

     //pthread_join(data->thread_1, NULL);
    return  data->thread_complete_success;
}

// pthread_t thread_g;
// pthread_mutex_t mutex_g;

// int main()
// {
//     int time;
//     bool ret = start_thread_obtaining_mutex(&thread_g, &mutex_g, 100, 100);
//     return 0;
// }

