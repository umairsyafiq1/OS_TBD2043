#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t tid[2];
int counter = 0;
pthread_mutex_t lock;

void* doSomeThing(void *arg)
{
    pthread_mutex_lock(&lock);

    counter++;
    printf("\nJob %d started\n", counter);

    volatile unsigned long i;
    for(i = 0; i < 0xFFFFFFFF; i++);   // delay loop

    printf("\nJob %d finished\n", counter);

    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(void)
{
    int i, err;

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex init failed\n");
        return 1;
    }

    for(i = 0; i < 2; i++) {
        err = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
        if (err != 0) {
            printf("Can't create thread: [%s]\n", strerror(err));
        }
    }

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    pthread_mutex_destroy(&lock);

    return 0;
}
