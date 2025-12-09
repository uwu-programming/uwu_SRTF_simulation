/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static int treasure = 1000;
static int count = 0;

pthread_mutex_t mutex;

void reduce1(){
    static int ite = 0;
    
    while (treasure > 0){
            //pthread_mutex_lock(&mutex);
            if(treasure>0){treasure--;
            //printf("1 treasure: %d \tite1: %d\n", treasure, ++ite);
            count++;}
            //pthread_mutex_unlock(&mutex);
    }
}

void reduce2(){
    static int ite = 0;
    
    while (treasure > 0){
            //pthread_mutex_lock(&mutex);

            if(treasure>0){treasure--;
            //printf("2 treasure: %d \tite2: %d\n", treasure, ++ite);
            count++;}
            //pthread_mutex_unlock(&mutex);

    }
}

void reduce3(){
    static int ite = 0;
    
    while (treasure > 0){
            //pthread_mutex_lock(&mutex);

            if(treasure>0){treasure--;
            //printf("3 treasure: %d \tite3: %d\n", treasure, ++ite);
            count++;}
            //pthread_mutex_unlock(&mutex);

    }
}

void reduce4(){
    static int ite = 0;
    
    while (treasure > 0){
            //pthread_mutex_lock(&mutex);

            if(treasure>0){treasure--;
            //printf("4 treasure: %d \tite4: %d\n", treasure, ++ite);
            count++;}
            //pthread_mutex_unlock(&mutex);

    }
}

int main()
{
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, (void*)(reduce1), NULL);
    pthread_create(&t2, NULL, (void*)(reduce2), NULL);
    pthread_create(&t3, NULL, (void*)(reduce3), NULL);
    pthread_create(&t4, NULL, (void*)(reduce4), NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    
    printf("\ncount:%d\n", count);
    
    return 0;
}