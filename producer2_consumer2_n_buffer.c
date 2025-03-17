#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define SIZE 5
#define MAX 100

static pthread_mutex_t mutex      = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond_prod  = PTHREAD_COND_INITIALIZER;
static pthread_cond_t  cond_cons  = PTHREAD_COND_INITIALIZER;

int buf[SIZE];
int counter = 0;
int index_prod = 0;
int index_cons = 0;
int prod_val = 1;
int cons_val = 1;
bool prod_done = false;

void* prod(void *p){
   for (;;){
      pthread_mutex_lock(&mutex);
      while(counter == SIZE){
         pthread_cond_wait(&cond_prod, &mutex);
      }
      if(prod_val <= MAX){
         buf[index_prod] = prod_val;
         counter++;
         prod_val++;
         index_prod = (index_prod + 1) % SIZE;
         pthread_cond_signal(&cond_cons);
         pthread_mutex_unlock(&mutex);
      }
      else{
         prod_done = true;
         pthread_cond_signal(&cond_cons);
         pthread_mutex_unlock(&mutex);
         break;
      }
   }
}

void* kons(void *p){
   for (;;){
      pthread_mutex_lock(&mutex);
      while(counter == 0 && !prod_done){
         pthread_cond_wait(&cond_cons, &mutex);
      }
      if(prod_done && counter == 0) {
         pthread_mutex_unlock(&mutex);
         break;
      }
      if(cons_val <= MAX){
         printf("%d\n", buf[index_cons]);
         counter--;
         cons_val++;
         index_cons = (index_cons + 1) % SIZE;
         pthread_cond_signal(&cond_prod);
         pthread_mutex_unlock(&mutex);
      }
      else{
         pthread_mutex_unlock(&mutex);
         break;
      }
   }
}

int main(){
   pthread_t pth1, pth2, pth3, pth4;
   pthread_create(&pth1, NULL, prod, NULL);
   pthread_create(&pth2, NULL, prod, NULL);
   pthread_create(&pth3, NULL, kons, NULL);
   pthread_create(&pth4, NULL, kons, NULL);
   pthread_join(pth1, NULL);
   pthread_join(pth2, NULL);
   pthread_join(pth3, NULL);
   pthread_join(pth4, NULL);
}
