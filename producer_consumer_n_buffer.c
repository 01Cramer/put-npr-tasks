#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX 5

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

int buf[MAX];
int counter = 0;

void* prod(void *p){
   int index = 0;
   int i;
   for(i=1;i<100;i++){
      pthread_mutex_lock(&mutex);
      while(counter == MAX){
         pthread_cond_wait(&cond, &mutex);
      }
      buf[index] = i;
      counter++;
      index = (index + 1) % MAX;
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&mutex);
   }
}

void* kons(void *p){
   int index = 0;
   int i;
   for(i=1;i<100;i++){
      pthread_mutex_lock(&mutex);
      while(counter == 0){
         pthread_cond_wait(&cond, &mutex);
      }
      printf("%d\n", buf[index]);
      counter--;
      index = (index + 1) % MAX;
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
