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
#include <sys/mman.h>
#include <sys/wait.h>

#define SIZE    2

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
		merge_sort(&left_block);
        merge_sort(&right_block);
        merge(&left_block, &right_block);
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
    
	long size;
	if (argc < 2) {
		size = SIZE;
	} else {
		size = atol(argv[1]);
	}
    struct block start_block;
    printf("IM MORBING \n");
    int *data = (int *)mmap(NULL, sizeof(int) * size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    printf("IM NOT MORBING\n");
    //int *data = (int *) calloc(size, sizeof(int));
    start_block.size = size;
    start_block.first = data;
    printf("I SEGFAULT HERE\n");
    for (int i = 0; i < size; i++) {
        data[i] = rand();
    }             
    printf("I SEGFAULT HERE?\n");
    struct block left_block;
    struct block right_block;
    //struct block my_data = start_block;
	left_block.size = start_block.size / 2;
    left_block.first = start_block.first;
    right_block.size = left_block.size + (start_block.size % 2);
    right_block.first = start_block.first + left_block.size;
    printf("I SEGFAULT HERE?!?\n");
    pid_t pid = fork();
    if (pid == -1){
        perror("CHILD HAS BIRTHED\n");
    }
    
    //pthread_create(&thread_refs[1], NULL, merge_sort, &right_block);
    if (pid != 0){
        printf("starting---\n");
        merge_sort(&left_block); 
        //read(my_pipe[0], right_block.first, sizeof(int) * right_block.size);
        wait(NULL);
        merge(&left_block,&right_block);
        printf("---ending\n");
        printf(is_sorted(data, size) ? "sorted\n" : "not sorted\n");
    } else {
        printf("THE CHILD IS BORN\n");
        merge_sort(&right_block);
        //write(my_pipe[1], right_block.first, sizeof(int)* right_block.size);
        printf("THE CHILD IS VANQUISHED\n");
    }
    


    
    
    
    exit(EXIT_SUCCESS);
}
