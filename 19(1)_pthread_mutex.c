#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    volatile unsigned long long counter;
    int iterations;
} work_area_t;

void * thr_body(void * arg) {
    work_area_t * w = arg;
    for (int i = 0; i < w->iterations; i++) {
        pthread_mutex_lock(&w->mutex);
        w->counter++;
        printf("counter(2) = %llu\n", w->counter);
        pthread_mutex_unlock(&w->mutex); 
    }
    puts("Done (secondary)");
    return NULL;
}

int main(int argc, char * argv[]) {
    work_area_t data = {
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .counter = 0,
        .iterations = (argc > 1) ? atoi(argv[1]) : 1000,
    };

    pthread_t secondary_thread_id;
    if(0 != pthread_create(&secondary_thread_id, NULL, thr_body, &data)) { //pthread_create returns 0 on success
        perror("pthread_create");
        return 1;
    }
    for (int i = 0; i < data.iterations; i++) {
        pthread_mutex_lock(&data.mutex);
        data.counter++;
        printf("counter(1) = %llu\n", data.counter);
        pthread_mutex_unlock(&data.mutex);
    }
    puts("Done (main)");
    pthread_join(secondary_thread_id, NULL);//как wait для процессов
    return 0;
}