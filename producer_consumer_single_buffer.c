#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

volatile int  buf;
volatile bool buf_empty = true;

void* prod(void *p){
   int i;
   for (i=1;i<100;i++){
      pthread_mutex_lock(&mutex);
      while(!buf_empty){
        pthread_cond_wait(&cond, &mutex);
      }
      buf = i;
      buf_empty = false;
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&mutex);
   }
}

void* kons(void *p){
   int i;
   for (i=1;i<100;i++){
    pthread_mutex_lock(&mutex);
    while(buf_empty){
      pthread_cond_wait(&cond, &mutex);
    }
    printf("%d\n", buf);
    buf_empty = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
   }
}

int main(){
   pthread_t pth1, pth2;
   pthread_create(&pth1, NULL, prod, NULL);
   pthread_create(&pth2, NULL, kons, NULL);
   pthread_join(pth1, NULL);
   pthread_join(pth2, NULL);
}