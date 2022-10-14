/*
    The Merge Sort to use for Operating Systems Assignment 1 2022
    written by Robert Sheehan

    Modified by: J Samuel Dosado
    UPI: jdos028

    By submitting a program you are claiming that you and only you have made
    adjustments and additions to this code.
 */

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <sys/resource.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#define SIZE    2
int thread_count = 1;
int cores;
pthread_mutex_t lock;
struct block {
    int size;
    int *first;
};

// void print_block_data(struct block *blk) {
//     printf("size: %d address: %p\n", blk->size, blk->first);
// }

/* Combine the two halves back together. */
void merge(struct block *left, struct block *right) {
	//int combined[left->size + right->size];
    int *combined = (int *) calloc((left->size + right->size), sizeof(int));
	int dest = 0, l = 0, r = 0;
	while (l < left->size && r < right->size) {
		if (left->first[l] < right->first[r])
			combined[dest++] = left->first[l++];
		else
			combined[dest++] = right->first[r++];
	}
	while (l < left->size)
		combined[dest++] = left->first[l++];
	while (r < right->size)
		combined[dest++] = right->first[r++];
    memmove(left->first, combined, (left->size + right->size) * sizeof(int));
    free(combined);
}

/* Merge sort the data. */
void *merge_sort(void* a) {
    struct block *my_data = (struct block*) a;
    // print_block_data(my_data);
    if (my_data->size > 1) {
        struct block left_block;
        struct block right_block;
        left_block.size = my_data->size / 2;
        left_block.first = my_data->first;
        right_block.size = left_block.size + (my_data->size % 2);
        right_block.first = my_data->first + left_block.size;
		if(thread_count >= cores){
            //printf("Reach thread limit of %d\n",get_nprocs_conf());
            merge_sort(&left_block);
            merge_sort(&right_block);
            merge(&left_block, &right_block);
            return NULL;
        }
		
        pthread_mutex_lock(&lock);
        //int thread_num = thread_count - 1;
        if (thread_count < cores){
            pthread_t t;
            thread_count ++;
            pthread_mutex_unlock(&lock);
            //printf("Created thread %d\n", thread_count);
            pthread_create(&t, NULL, merge_sort, &right_block);
            merge_sort(&left_block);
            pthread_join(t, NULL);
            //printf("removed thread$%d\n",thread_num);
            //thread_count --;
            merge(&left_block, &right_block);
            //printf("Created thread %d\n", thread_num);
            //thread_count ++;
        } else {
            pthread_mutex_unlock(&lock);
            merge_sort(&left_block);
            merge_sort(&right_block);
            merge(&left_block, &right_block);
            //return NULL;
        }
    }
}

/* Check to see if the data is sorted. */
bool is_sorted(int data[], int size) {
    bool sorted = true;
    for (int i = 0; i < size - 1; i++) {
        if (data[i] > data[i + 1])
            sorted = false;
    }
    return sorted;
}

int main(int argc, char *argv[]) {
	//int const num_threads = 10;
	//pthread_t thread_refs[SIZE];
    cores = get_nprocs_conf();
	pthread_mutex_init(&lock, NULL);
	long size;

	if (argc < 2) {
		size = SIZE;
	} else {
		size = atol(argv[1]);
	}
    struct block start_block;
    int *data = (int *) calloc(size, sizeof(int));
    start_block.size = size;
    start_block.first = data;
    for (int i = 0; i < size; i++) {
        data[i] = rand();
    }             
    
    printf("starting---\n");
    
    //pthread_create(&thread_refs[1], NULL, merge_sort, &right_block);
    merge_sort(&start_block);
    printf("---ending\n");
    printf(is_sorted(data, size) ? "sorted\n" : "not sorted\n");
    free(data);
    exit(EXIT_SUCCESS);
}
