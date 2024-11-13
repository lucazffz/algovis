#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int* arr, int left_idx, int right_idx) {
    int pivot_idx = left_idx;

    int i = left_idx + 1; // first element after pivot
    int j = right_idx;

    // pivot is first element so no initial swap neccecary
    while (true) {
        // could swap if equals pivot but would be unneccecary work
        while (arr[i] <= arr[pivot_idx] && i < right_idx) {
            i++;
        }

        while (arr[j] >= arr[pivot_idx] && j > left_idx) {
            j--;
        }

        // need check here because should not swap if i >= j which occurs
        // on the last iteration
        if (i < j) {
            swap(&arr[i], &arr[j]);
        } else {
            // place pivot at right index
            swap(&arr[left_idx], &arr[j]);
            return j;
        }
    }
}

void quick_sort_help(int* arr, int left_idx, int right_idx) {
    if (left_idx >= right_idx) {
        return;
    }

    int pivot_idx = partition(arr, left_idx, right_idx);

    // when we return if the call stask the array will already be sorted
    // know that pivot is already at the correct index
    quick_sort_help(arr, left_idx, pivot_idx - 1);
    quick_sort_help(arr, pivot_idx + 1, right_idx);
}

void quick_sort(int* arr, int len) {
    quick_sort_help(arr, 0, len - 1);
}

#define ARR_LEN 20

int main() {

    int arr[ARR_LEN];
    for (int i = 0; i < ARR_LEN; i++) {
        arr[i] = rand() % 100;
    }

    printf("Original array: ");
    for (int i = 0; i < ARR_LEN; i++) {
        printf("%d, ", arr[i]);
    }
    clock_t start, end;

    start = clock();
    quick_sort(arr, ARR_LEN);
    end = clock();

    printf("\nSorted array: ");
    for (int i = 0; i < ARR_LEN; i++) {
        printf("%d, ", arr[i]);
    }

    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nSorting took %f seconds", cpu_time_used);
    return 0;
}
